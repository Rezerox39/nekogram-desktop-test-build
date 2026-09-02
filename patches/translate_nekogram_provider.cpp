/*
This file is part of Nekogram Desktop.

Google Translate provider (free, client=gtx) and DeepL free API.
Yandex free API is deprecated; the option is intentionally omitted.
Runs on the calling thread's QNetworkAccessManager, never blocks the UI.

Lifetime: the provider is owned by TranslateBox via
box->lifetime().make_state<State>().  When the box is destroyed, the
provider's QNetworkAccessManager is destroyed, disconnecting all pending
reply signals.  Stock UrlTranslateProvider uses the same pattern with
a plain [=] capture.

Telegram MTProto mode is NOT routed through this provider.  The factory
(CreateTranslateProvider) returns CreateMTProtoTranslateProvider directly
when Neko::translateProviderType() == Telegram.
*/
#include "lang/translate_nekogram_provider.h"

#include "neko/neko_config.h"
#include "spellcheck/platform/platform_language.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Ui {
namespace {

// ---- language detection ------------------------------------------------

[[nodiscard]] QString DetectFromLanguage(const QString &text) {
#ifndef TDESKTOP_DISABLE_SPELLCHECK
	const auto result = Platform::Language::Recognize(text);
	return result.known() ? result.twoLetterCode() : u"auto"_q;
#else
	return u"auto"_q;
#endif
}

// ---- Google Translate (free, client=gtx) -------------------------------

[[nodiscard]] QUrl BuildGoogleUrl(
		const QString &text,
		const QString &to) {
	auto url = QUrl(u"https://translate.googleapis.com/translate_a/single"_q);
	auto query = QUrlQuery();
	query.addQueryItem(u"client"_q, u"gtx"_q);
	query.addQueryItem(u"sl"_q, u"auto"_q);
	query.addQueryItem(u"tl"_q, to);
	query.addQueryItem(u"dt"_q, u"t"_q);
	query.addQueryItem(u"q"_q, text);
	url.setQuery(query);
	return url;
}

[[nodiscard]] std::optional<QString> ParseGoogleResponse(
		const QByteArray &body) {
	auto error = QJsonParseError();
	const auto doc = QJsonDocument::fromJson(body, &error);
	if (error.error != QJsonParseError::NoError || !doc.isArray()) {
		return std::nullopt;
	}
	const auto root = doc.array();
	if (root.isEmpty() || !root[0].isArray()) {
		return std::nullopt;
	}
	auto result = QString();
	const auto segments = root[0].toArray();
	for (const auto &segment : segments) {
		if (!segment.isArray()) {
			return std::nullopt;
		}
		const auto pair = segment.toArray();
		if (pair.isEmpty() || !pair[0].isString()) {
			return std::nullopt;
		}
		result += pair[0].toString();
	}
	return result.trimmed().isEmpty()
		? std::nullopt
		: std::make_optional(result);
}

// ---- DeepL free API ---------------------------------------------------

[[nodiscard]] QUrl BuildDeepLUrl() {
	return QUrl(u"https://api-free.deepl.com/v2/translate"_q);
}

[[nodiscard]] QByteArray BuildDeepLBody(
		const QString &text,
		const QString &to) {
	QUrlQuery params;
	params.addQueryItem(u"text"_q, text);
	params.addQueryItem(u"target_lang"_q, to.toUpper());
	return params.toString(QUrl::FullyEncoded).toUtf8();
}

[[nodiscard]] std::optional<QString> ParseDeepLResponse(
		const QByteArray &body) {
	auto error = QJsonParseError();
	const auto doc = QJsonDocument::fromJson(body, &error);
	if (error.error != QJsonParseError::NoError || !doc.isObject()) {
		return std::nullopt;
	}
	const auto obj = doc.object();
	if (obj.value(u"message"_q).isString()) {
		return std::nullopt;
	}
	const auto translations = obj.value(u"translations"_q).toArray();
	if (translations.isEmpty()) {
		return std::nullopt;
	}
	const auto text = translations[0].toObject()
		.value(u"text"_q).toString();
	return text.trimmed().isEmpty()
		? std::nullopt
		: std::make_optional(text);
}

// ---- provider implementation ------------------------------------------

class NekoTranslateProvider final : public TranslateProvider {
public:
	NekoTranslateProvider() = default;

	[[nodiscard]] bool supportsMessageId() const override {
		return false;
	}

	void request(
			TranslateProviderRequest request,
			LanguageId to,
			Fn<void(TranslateProviderResult)> done) override {
		if (request.text.text.isEmpty()) {
			done(TranslateProviderResult{
				.error = TranslateProviderError::Unknown,
			});
			return;
		}

		const auto provider = Neko::translateProviderType();
		Expects(provider != Neko::TranslateProviderType::Telegram);

		const auto toCode = to.twoLetterCode();
		switch (provider) {
		case Neko::TranslateProviderType::DeepL: {
			const auto key = Neko::deepLApiKey();
			if (key.isEmpty()) {
				requestGoogle(request.text.text, toCode, done);
			} else {
				requestDeepL(key, request.text.text, toCode, done);
			}
			break;
		}
		case Neko::TranslateProviderType::Google:
		default:
			requestGoogle(request.text.text, toCode, done);
			break;
		}
	}

private:
	// ---- Google --------------------------------------------------------

	void requestGoogle(
			const QString &text,
			const QString &to,
			Fn<void(TranslateProviderResult)> done) {
		const auto url = BuildGoogleUrl(text, to);
		if (!url.isValid()) {
			done(TranslateProviderResult{
				.error = TranslateProviderError::Unknown,
			});
			return;
		}
		auto networkRequest = QNetworkRequest(url);
		const auto reply = _network.get(networkRequest);
		QObject::connect(reply, &QNetworkReply::finished, [=] {
			auto result = TranslateProviderResult();
			if (reply->error() != QNetworkReply::NoError) {
				result.error = TranslateProviderError::Unknown;
			} else {
				const auto body = reply->readAll();
				const auto parsed = ParseGoogleResponse(body);
				if (parsed) {
					result.text = TextWithEntities{ *parsed };
				} else {
					result.error = TranslateProviderError::Unknown;
				}
			}
			done(std::move(result));
			reply->deleteLater();
		});
	}

	// ---- DeepL ---------------------------------------------------------

	void requestDeepL(
			const QString &apiKey,
			const QString &text,
			const QString &to,
			Fn<void(TranslateProviderResult)> done) {
		auto networkRequest = QNetworkRequest(BuildDeepLUrl());
		networkRequest.setHeader(
			QNetworkRequest::ContentTypeHeader,
			"application/x-www-form-urlencoded");
		networkRequest.setRawHeader(
			"Authorization",
			("DeepL-Auth-Key " + apiKey).toUtf8());

		const auto body = BuildDeepLBody(text, to);
		const auto reply = _network.post(networkRequest, body);
		QObject::connect(reply, &QNetworkReply::finished, [=] {
			auto result = TranslateProviderResult();
			if (reply->error() != QNetworkReply::NoError) {
				result.error = TranslateProviderError::Unknown;
			} else {
				const auto responseBody = reply->readAll();
				const auto parsed = ParseDeepLResponse(responseBody);
				if (parsed) {
					result.text = TextWithEntities{ *parsed };
				} else {
					result.error = TranslateProviderError::Unknown;
				}
			}
			done(std::move(result));
			reply->deleteLater();
		});
	}

	QNetworkAccessManager _network;
};

} // namespace

std::unique_ptr<TranslateProvider> CreateNekoTranslateProvider() {
	return std::make_unique<NekoTranslateProvider>();
}

} // namespace Ui
