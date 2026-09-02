/*
This file is part of Nekogram Desktop.

Free translation provider backed by external HTTP APIs.
Routes translation calls through HTTP instead of Telegram's
Premium-gated messages.translateText endpoint.
*/
#pragma once

#include "lang/translate_provider.h"

namespace Ui {

[[nodiscard]] std::unique_ptr<TranslateProvider>
CreateNekoTranslateProvider();

} // namespace Ui
