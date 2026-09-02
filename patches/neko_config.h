/*
This file is part of Nekogram Desktop.

Centralised Nekogram-specific settings, persisted via base::options.
Each option is a standalone value stored in the user data directory;
no modifications to Core::Settings required.
*/
#pragma once

#include <QtCore/QString>

namespace Neko {

enum class TranslateProviderType : int {
	Google = 0,
	DeepL = 1,
	Telegram = 2,
};

[[nodiscard]] TranslateProviderType translateProviderType();
void setTranslateProviderType(TranslateProviderType type);

[[nodiscard]] QString translationTargetLanguage();
void setTranslationTargetLanguage(const QString &lang);

[[nodiscard]] QString deepLApiKey();
void setDeepLApiKey(const QString &key);

} // namespace Neko
