cmake_minimum_required(VERSION 3.22)

project(synthesizer_practice VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# JUCE の取り込み
# -----------------------------------------------------------------------------
# 優先順位:
#   1. CMake オプション -DJUCE_PATH=... で指定（ローカルクローン）
#   2. 環境変数 JUCE_PATH
#   3. それ以外は FetchContent でビルド時にダウンロード
#
# 並行開発するなら、各 worktree で JUCE をダウンロードしないよう
# -DJUCE_PATH=C:/dev/JUCE のように共通の場所を指定するのを推奨。

if(DEFINED JUCE_PATH)
  message(STATUS "Using local JUCE at: ${JUCE_PATH}")
  add_subdirectory("${JUCE_PATH}" JUCE)
elseif(DEFINED ENV{JUCE_PATH})
  message(STATUS "Using local JUCE from env: $ENV{JUCE_PATH}")
  add_subdirectory("$ENV{JUCE_PATH}" JUCE)
else()
  message(STATUS "JUCE not specified, fetching via FetchContent (slow on first build)")
  include(FetchContent)
  FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
  )
  FetchContent_MakeAvailable(JUCE)
endif()

# -----------------------------------------------------------------------------
# プラグイン定義
# -----------------------------------------------------------------------------
juce_add_plugin(synthesizer_practice
  COMPANY_NAME              "fewlio"
  PLUGIN_MANUFACTURER_CODE  Flio
  PLUGIN_CODE               SyPr
  FORMATS                   Standalone VST3
  PRODUCT_NAME              "Synthesizer Practice"
  IS_SYNTH                  TRUE
  NEEDS_MIDI_INPUT          TRUE
  NEEDS_MIDI_OUTPUT         FALSE
  IS_MIDI_EFFECT            FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD   FALSE
  VST3_CATEGORIES           "Instrument" "Synth"
)

# JUCE ヘッダー生成
juce_generate_juce_header(synthesizer_practice)

# -----------------------------------------------------------------------------
# 日本語フォントを埋め込みリソースとして組み込む
# システムフォント解決を迂回し、確実に日本語を描画するため。
# Noto Sans JP は SIL Open Font License (OFL) — Resources/OFL.txt 参照。
# -----------------------------------------------------------------------------
juce_add_binary_data(synth_fonts
  HEADER_NAME BinaryData.h
  NAMESPACE BinaryData
  SOURCES
    Resources/NotoSansJP-Regular.ttf
)

# -----------------------------------------------------------------------------
# ソースファイル
# 各セッションが追加する dsp/* や ui/* ファイルは
# CMakeLists.txt の glob ではなく明示的に書くこと（マージ衝突を避けるため）
# -----------------------------------------------------------------------------
target_sources(synthesizer_practice
  PRIVATE
    # コア
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Parameters.cpp

    # DSP（Session A 担当）
    Source/dsp/SynthVoice.cpp
    Source/dsp/Oscillator.cpp
    Source/dsp/Filter.cpp
    Source/dsp/Envelope.cpp
    Source/dsp/LFO.cpp
    Source/dsp/Noise.cpp
    Source/dsp/Drive.cpp
    Source/dsp/Delay.cpp
    Source/dsp/Reverb.cpp

    # UI プリミティブ（Session B）
    Source/ui/CustomLookAndFeel.cpp
    Source/ui/RotaryKnob.cpp
    Source/ui/SectionPanel.cpp

    # ビジュアライザ（Session C）
    Source/ui/WaveformDisplay.cpp
    Source/ui/SpectrumDisplay.cpp

    # Editor + セクションパネル（Session D）
    Source/ui/InfoPanel.cpp
    Source/ui/OscillatorPanel.cpp
    Source/ui/NoisePanel.cpp
    Source/ui/FilterPanel.cpp
    Source/ui/FilterEnvelopePanel.cpp
    Source/ui/AmpEnvelopePanel.cpp
    Source/ui/LfoPanel.cpp
    Source/ui/DrivePanel.cpp
    Source/ui/DelayPanel.cpp
    Source/ui/ReverbPanel.cpp
    Source/ui/VoicePanel.cpp
    Source/ui/MasterPanel.cpp

    # パラメータ解説 + プリセット（Session E）
    Source/PresetManager.cpp
    Source/Descriptions.cpp
)

# Source/ をインクルードパスに追加
target_include_directories(synthesizer_practice
  PRIVATE
    Source
)

# JUCE モジュールとリンク
target_link_libraries(synthesizer_practice
  PRIVATE
    synth_fonts
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

target_compile_definitions(synthesizer_practice
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# MSVC: UTF-8 でソース・実行時文字列を扱う（C4819 警告と日本語文字列リテラルの誤解釈を解消）
if(MSVC)
  target_compile_options(synthesizer_practice PRIVATE /utf-8)
endif()
