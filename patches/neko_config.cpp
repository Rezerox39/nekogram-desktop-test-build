/*
This file is part of Nekogram Desktop.

Nekogram settings backed by base::options — the same mechanism tdesktop
uses for --flags and translate-url-template.  Values persist across
restarts in the user data directory's options.json.
*/
#include "neko/neko_config.h"

#include "base/options.h"

namespace Neko {
namespace {

base::options::option<int> OptionTranslateProvider({
	.id = "nekogram-translate-provider",
	.name = "Nekogram translate provider",
	.description = "0 = Google, 1 = DeepL, 2 = Telegram MTProto",
});

base::options::option<QString> OptionTranslationTarget({
	.id = "nekogram-translation-target",
	.name = "Nekogram translation target language",
	.description = "Two-letter target language code (e.g. en, ru). "
		"Empty means system language.",
});

base::options::option<QString> OptionDeepLApiKey({
	.id = "nekogram-deepl-api-key",
	.name = "Nekogram DeepL API key",
	.description = "DeepL free API key (get from deepl.com/pro-api).",
});

} // namespace

TranslateProviderType translateProviderType() {
	const auto raw = OptionTranslateProvider.value();
	if (raw < 0 || raw > int(TranslateProviderType::Telegram)) {
		return TranslateProviderType::Google;
	}
	return TranslateProviderType(raw);
}

void setTranslateProviderType(TranslateProviderType type) {
	OptionTranslateProvider.set(int(type));
}

QString translationTargetLanguage() {
	return OptionTranslationTarget.value();
}

void setTranslationTargetLanguage(const QString &lang) {
	OptionTranslationTarget.set(lang);
}

QString deepLApiKey() {
	return OptionDeepLApiKey.value();
}

void setDeepLApiKey(const QString &key) {
	OptionDeepLApiKey.set(key);
}

} // namespace Neko
