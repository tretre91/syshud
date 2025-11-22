#pragma once

// Build time configuration				Description
// #define RUNTIME_CONFIG					// Allow the use of runtime arguments
#define CONFIG_FILE						// Allow the use of a config file
// #define AUDIO_WIREPLUMBER				// Use wireplumber as the audio backend
#define AUDIO_PULSEAUDIO				// Use pulseaudio as the audio backend
#define FEATURE_BACKLIGHT				// Include backlight monitor
#define FEATURE_KEYBOARD				// Include keyboard status indicator

// Icons
#define ICON_CAPS_LOCK_ON "capslock-enabled-symbolic"
#define ICON_CAPS_LOCK_OFF "capslock-disabled-symbolic"

#define ICON_NUM_LOCK_ON "numlock-enabled-symbolic"
#define ICON_NUM_LOCK_OFF "numlock-disabled-symbolic"

#define ICON_SPEAKER_MUTED "audio-volume-muted-symbolic"
#define ICON_SPEAKER_LOW "audio-volume-low-symbolic"
#define ICON_SPEAKER_MEDIUM "audio-volume-medium-symbolic"
#define ICON_SPEAKER_HIGH "audio-volume-high-symbolic"
#define ICON_SPEAKER_OVERAMPLIFIED "audio-volume-overamplified-symbolic"

#define ICON_MIC_MUTED "audio-input-microphone-muted-symbolic"
#define ICON_MIC_LOW "audio-input-microphone-low-symbolic"
#define ICON_MIC_MEDIUM "audio-input-microphone-medium-symbolic"
#define ICON_MIC_HIGH "audio-input-microphone-high-symbolic"

#define ICON_BRIGHTNESS_OFF "display-brightness-off-symbolic"
#define ICON_BRIGHTNESS_LOW "display-brightness-low-symbolic"
#define ICON_BRIGHTNESS_MEDIUM "display-brightness-medium-symbolic"
#define ICON_BRIGHTNESS_HIGH "display-brightness-high-symbolic"
