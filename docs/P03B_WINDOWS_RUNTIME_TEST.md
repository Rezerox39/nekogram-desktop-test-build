# P0-3b Windows Runtime Test Document

## Build Info
- **Build type**: NON_PRODUCTION_TEST_BUILD_NO_CRASH_REPORTS
- **tdesktop commit**: 087b18b89194ab474526ca61b7992f32f52c983b
- **Crash reporting**: Disabled
- **Platform**: Windows x64

## Test Procedure

1. Launch `Telegram.exe`
2. Complete initial setup (phone number, etc.)
3. Navigate to **Settings → Language/Translate**
4. Click **Nekogram Translation**
5. Run through the tests below

## Test Cases

### Navigation
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Settings → Language/Translate → Nekogram Translation | Opens Nekogram Translation settings panel | | | |
| Panel opens and displays provider list | Three providers shown: Google, DeepL, Telegram | | | |
| Click "Close" button | Panel closes, returns to Settings | | | |
| Press Escape key | Panel closes | | | |
| Press Back button | Panel closes | | | |

### Provider Selection
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Click "Google Translate (free)" | Checkmark appears next to Google | | | |
| Click "DeepL" | Checkmark moves to DeepL | | | |
| Click "Telegram MTProto (Premium)" | Checkmark moves to Telegram | | | |
| Close and reopen panel | Selected provider persists | | | |
| Restart app and reopen panel | Selected provider persists | | | |

### Google Translation
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Set provider to Google | Google selected | | | |
| Right-click a message → Translate | Translation appears | | | |
| Translate English text to Russian | Russian translation displayed | | | |
| Translate Russian text to English | English translation displayed | | | |
| Translate multiline text | All lines translated | | | |
| Translate text with URLs | URLs preserved in translation | | | |

### DeepL Translation
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Click DeepL API key row | Key entry dialog opens | | | |
| Enter valid DeepL API key | Key masked (dots/asterisks) | | | |
| Click Save | Key saved, dialog closes | | | |
| Reopen key dialog | Saved key displayed (masked) | | | |
| Click Cancel | Old key preserved, dialog closes | | | |
| Enter empty key and save | Key cleared | | | |
| Enter invalid characters | Error shown, not saved | | | |
| Set provider to DeepL, translate | Translation via DeepL API | | | |
| Remove API key, set provider to DeepL | Falls back to Google | | | |

### Target Language
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Click "Target Language" | Language picker opens | | | |
| Select English | Language set to English | | | |
| Select Russian | Language set to Russian | | | |
| Select "System language" | Uses system default | | | |
| Close and reopen panel | Language persists | | | |
| Restart app and reopen | Language persists | | | |

### Language Variants
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Select Simplified Chinese | Translations use Simplified Chinese | | | |
| Select Traditional Chinese | Translations use Traditional Chinese | | | |
| Select Brazilian Portuguese | Translations use Brazilian Portuguese | | | |
| Select Arabic | Translations use Arabic, RTL text | | | |
| Select a European language (e.g., German) | Translations use that language | | | |

### Error Handling
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Translate with no internet | Error message shown | | | |
| Translate empty message | No crash, no translation | | | |
| Cancel translation mid-request | No crash | | | |
| Close UI during active request | No crash | | | |

### Themes and Scaling
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| Test in light theme | UI renders correctly | | | |
| Test in dark theme | UI renders correctly | | | |
| Test at 100% scaling | UI renders correctly | | | |
| Test at 125% scaling | UI renders correctly | | | |
| Test at 150% scaling | UI renders correctly | | | |
| Test at 200% scaling | UI renders correctly | | | |

### Stock Behavior Preservation
| Test | Expected | Actual | Pass/Fail | Notes |
|------|----------|--------|-----------|-------|
| URL-template provider still works | If configured, URL template takes priority | | | |
| Platform provider still works | CrowTranslate etc. still functional | | | |
| Telegram MTProto mode | Stock Premium-gated behavior | | | |
| No secret leakage | Keys not in logs, errors, clipboard | | | |

## Screenshot Procedure

For each test, take a screenshot showing:
1. The exact UI state (panel open, translation result, etc.)
2. Note any objective issues: clipping, misalignment, missing strings, wrong row heights, theme problems, broken navigation.

Do NOT judge whether the UI "looks like Telegram" — only report objective functional issues.

## Notes

- This is a NON_PRODUCTION_TEST_BUILD with crash reporting disabled.
- The DeepL API key is stored in plain text (options.json), not OS credential store.
- Telegram's public test API credentials are used (rate-limited).
- Language selection uses 2-letter codes; regional variants (zh-CN vs zh-TW, pt-BR vs pt-PT) default to the primary variant.
