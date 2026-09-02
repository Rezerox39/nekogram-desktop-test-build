# Nekogram Desktop — Temporary Public Test Build

**NON_PRODUCTION_TEST_BUILD_NO_CRASH_REPORTS**

This is a temporary public build repository for an unofficial, non-production 
Nekogram-inspired Telegram Desktop test build. This project is **not affiliated 
with or endorsed by Telegram FZ-LLC or the Nekogram project.**

## ⚠️ Disclaimer

- **Crash reporting is disabled** in this test build.
- **Do not use this build for production or sensitive communication.**
- This is a test build only — features may be incomplete or unstable.
- API credentials used are Telegram's public test credentials (rate-limited).
- DeepL API keys are stored locally in plain text (not OS credential store).

## What This Build Does

This test build adds free translation (Google Translate, DeepL) to Telegram Desktop
by patching the official tdesktop source with Nekogram-inspired features:

- **Free translation** — bypasses Telegram Premium requirement
- **Google Translate** — free, no API key needed (client=gtx)
- **DeepL** — free API key required (get from deepl.com/pro-api)
- **Telegram MTProto** — falls back to stock Premium-gated behavior
- **Settings panel** — reachable from Settings → Language/Translate

## Build Details

- **tdesktop base**: commit `087b18b89194ab474526ca61b7992f32f52c983b` (dev branch)
- **Crash reporting**: Disabled (`DESKTOP_APP_DISABLE_CRASH_REPORTS=ON`)
- **Platform**: Windows x64
- **Test credentials**: Telegram's official public test API credentials

## How to Build

See `.github/workflows/windows-test-build.yml` for the complete Windows build process.

## License

This project inherits the tdesktop license. See the tdesktop repository for full
license details: https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL

## Attribution

- Telegram Desktop: https://github.com/telegramdesktop/tdesktop
- Nekogram: https://github.com/Nekogram/Nekogram
- Desktop App Toolkit: https://github.com/desktop-app/cmake_helpers
