/*
This file is part of Nekogram Desktop.

Nekogram translation settings dialog (reachable from
Settings → Language/Translate → Nekogram Translation).

Reuses real tdesktop components:
  - Ui::GenericBox for the dialog container
  - Ui::ChooseLanguageBox for the target-language picker
  - Ui::PasswordInput for the masked API key entry
  - Ui::vertical_list / vertical_layout helpers for rows
  - st::boxRowPadding / st::defaultInputField style constants
*/
#include "neko/neko_translate_settings.h"
#include "neko/neko_config.h"

#include "core/application.h"
#include "lang/lang_keys.h"
#include "spellcheck/spellcheck_types.h"
#include "ui/boxes/choose_language_box.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/password_input.h"
#include "ui/wrap/vertical_layout.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"

namespace Neko {
namespace {

constexpr auto kMaxDeepLKeyLength = 120;

enum class RowState {
	Active,
	Inactive,
};

void AddProviderRow(
		not_null<Ui::VerticalLayout*> content,
		TranslateProviderType type,
		RowState state) {
	const auto active = (state == RowState::Active);
	const auto prefix = active ? u"\u2713  "_q : u"    "_q;
	const auto label = prefix + [type] {
		switch (type) {
		case TranslateProviderType::Google:   return tr::lng_neko_provider_google(tr::now);
		case TranslateProviderType::DeepL:    return tr::lng_neko_provider_deepl(tr::now);
		case TranslateProviderType::Telegram: return tr::lng_neko_provider_telegram(tr::now);
		}
		return QString();
	}();

	const auto button = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(label),
			st::settingsButtonNoIcon));
	button->addClickHandler([type] {
		setTranslateProviderType(type);
	});
}

[[nodiscard]] QString CurrentTargetLabel() {
	const auto code = translationTargetLanguage();
	if (code.isEmpty()) {
		return tr::lng_neko_system_language(tr::now);
	}
	const auto map = QLocale(); // not used; fallback below
	QLocale locale(code);
	return locale.language() != QLocale::C
		? QLocale::languageToString(locale.language())
		: code;
}

void ShowLanguagePicker(not_null<Ui::GenericBox*> box) {
	const auto current = translationTargetLanguage();
	LanguageId currentId;
	if (!current.isEmpty()) {
		currentId = LanguageId::FromName(current);
	}
	auto selected = std::vector<LanguageId>();
	if (currentId.known()) {
		selected.push_back(currentId);
	}

	box->uiShow()->showBox(
		Box(Ui::ChooseLanguageBox,
			tr::lng_languages(),
			[=](std::vector<LanguageId> &&ids) {
				Expects(!ids.empty());
				const auto id = ids.front();
				setTranslationTargetLanguage(
					QString::fromLatin1(id.twoLetterCode()));
			},
			selected,
			false,
			nullptr));
}

void ShowApiKeyDialog(not_null<Ui::GenericBox*> box) {
	box->uiShow()->showBox(
		Box([=](not_null<Ui::GenericBox*> keyBox) {
			keyBox->setTitle(
				tr::lng_neko_deepL_key_title());
			const auto content = keyBox->verticalLayout();

			const auto input = content->add(
				object_ptr<Ui::PasswordInput>(
					content,
					st::defaultInputField,
					tr::lng_neko_deepL_key_placeholder(),
					deepLApiKey()));
			input->setMaxLength(kMaxDeepLKeyLength);

			keyBox->addButton(tr::lng_settings_save(), [=] {
				const auto key = input->getLastText().trimmed();
				const auto empty = key.isEmpty();
				// Reject obvious malformed input (only [A-Za-z0-9:_-]).
				const auto malformed = std::any_of(
					key.begin(),
					key.end(),
					[](QChar c) {
						return !(c.isLetterOrNumber()
							|| c == u':'
							|| c == u'_'
							|| c == u'-');
					});
				if (empty) {
					setDeepLApiKey(QString());
					keyBox->closeBox();
					return;
				}
				if (malformed) {
					input->showError();
					return;
				}
				setDeepLApiKey(key);
				keyBox->closeBox();
			});
			keyBox->addButton(tr::lng_cancel(), [=] {
				// Preserve old value on cancel.
				keyBox->closeBox();
			});
		}));
}

} // namespace

void NekoTranslationSettingsBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_neko_translation_settings());

	const auto content = box->verticalLayout();

	Ui::AddSubsectionTitle(
		content,
		tr::lng_neko_translation_provider());

	AddProviderRow(content, TranslateProviderType::Google,
		(translateProviderType() == TranslateProviderType::Google)
			? RowState::Active : RowState::Inactive);
	AddProviderRow(content, TranslateProviderType::DeepL,
		(translateProviderType() == TranslateProviderType::DeepL)
			? RowState::Active : RowState::Inactive);
	AddProviderRow(content, TranslateProviderType::Telegram,
		(translateProviderType() == TranslateProviderType::Telegram)
			? RowState::Active : RowState::Inactive);

	Ui::AddDivider(content);

	// Target language (uses real ChooseLanguageBox picker).
	{
		const auto btn = content->add(
			object_ptr<Ui::SettingsButton>(
				content,
				tr::lng_neko_target_language(),
				st::settingsButtonNoIcon),
			st::boxRowPadding);
		btn->addClickHandler([=] {
			ShowLanguagePicker(box);
		});
	}

	Ui::AddDivider(content);

	// DeepL API key row.
	{
		const auto keySet = !deepLApiKey().isEmpty();
		const auto label = keySet ? tr::lng_neko_deepL_key_configured(tr::now) : tr::lng_neko_deepL_key_not_set(tr::now);
		const auto btn = content->add(
			object_ptr<Ui::SettingsButton>(
				content,
				rpl::single(label),
				st::settingsButtonNoIcon),
			st::boxRowPadding);
		btn->addClickHandler([=] {
			ShowApiKeyDialog(box);
		});
	}

	Ui::AddSkip(content);

	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace Neko
