import { useState, useEffect, useRef, useCallback } from 'react';
import { OscillatorSection } from './components/sections/OscillatorSection';
import { NoiseSection } from './components/sections/NoiseSection';
import { FilterSection } from './components/sections/FilterSection';
import { EnvelopeSection } from './components/sections/EnvelopeSection';
import { FilterEnvelopeSection } from './components/sections/FilterEnvelopeSection';
import { LFOSection } from './components/sections/LFOSection';
import { DriveSection } from './components/sections/DriveSection';
import { DelaySection } from './components/sections/DelaySection';
import { ReverbSection } from './components/sections/ReverbSection';
import { VoiceSection } from './components/sections/VoiceSection';
import { MasterSection } from './components/sections/MasterSection';
import { Keyboard } from './components/Keyboard';
import { WaveformView } from './components/WaveformView';
import { SpectrumView } from './components/SpectrumView';
import { InfoPanel } from './components/InfoPanel';
import { PresetBrowser } from './components/PresetBrowser';
import { SynthEngine } from './audio/SynthEngine';
import { SynthEngineProvider } from './audio/engineContext';
import type { SynthPreset } from './types';
import {
  DEFAULT_OSCILLATOR,
  DEFAULT_NOISE,
  DEFAULT_FILTER,
  DEFAULT_ENVELOPE,
  DEFAULT_FILTER_ENVELOPE,
  DEFAULT_LFO,
  DEFAULT_DRIVE,
  DEFAULT_DELAY,
  DEFAULT_REVERB,
  DEFAULT_UNISON,
  DEFAULT_PERFORMANCE,
  DEFAULT_MASTER_VOLUME,
} from './data/defaults';

/**
 * プリセット適用やリセット時にエンジンへ反映するすべてのパラメータ。
 * 各 Section の初期値（initialParams）にも同じものを渡す。
 */
interface AllParams {
  oscillator: typeof DEFAULT_OSCILLATOR;
  noise: typeof DEFAULT_NOISE;
  filter: typeof DEFAULT_FILTER;
  envelope: typeof DEFAULT_ENVELOPE;
  filterEnvelope: typeof DEFAULT_FILTER_ENVELOPE;
  lfo: typeof DEFAULT_LFO;
  drive: typeof DEFAULT_DRIVE;
  delay: typeof DEFAULT_DELAY;
  reverb: typeof DEFAULT_REVERB;
  unison: typeof DEFAULT_UNISON;
  performance: typeof DEFAULT_PERFORMANCE;
  masterVolume: number;
}

const ALL_DEFAULTS: AllParams = {
  oscillator: DEFAULT_OSCILLATOR,
  noise: DEFAULT_NOISE,
  filter: DEFAULT_FILTER,
  envelope: DEFAULT_ENVELOPE,
  filterEnvelope: DEFAULT_FILTER_ENVELOPE,
  lfo: DEFAULT_LFO,
  drive: DEFAULT_DRIVE,
  delay: DEFAULT_DELAY,
  reverb: DEFAULT_REVERB,
  unison: DEFAULT_UNISON,
  performance: DEFAULT_PERFORMANCE,
  masterVolume: DEFAULT_MASTER_VOLUME,
};

// アプリのルート。各セクションの統合点。
// AudioContext は autoplay policy のため、ユーザー操作（クリック）後に init する。
export default function App() {
  const [hoveredParam, setHoveredParam] = useState<string | null>(null);
  const [engine, setEngine] = useState<SynthEngine | null>(null);
  const engineRef = useRef<SynthEngine | null>(null);
  const [initError, setInitError] = useState<string | null>(null);

  // プリセット適用時の現在パラメータ。各 Section の initialParams に渡す
  const [params, setParams] = useState<AllParams>(ALL_DEFAULTS);
  // パラメータが変わったら全 Section を再マウントして state を初期化する
  const [presetVersion, setPresetVersion] = useState(0);

  const handleStart = async () => {
    if (engineRef.current) return;
    try {
      const e = new SynthEngine();
      await e.init();
      engineRef.current = e;
      setEngine(e);
    } catch (err) {
      setInitError(err instanceof Error ? err.message : String(err));
    }
  };

  // アンマウント時にリソース解放
  useEffect(() => {
    return () => {
      engineRef.current?.dispose();
      engineRef.current = null;
    };
  }, []);

  /** すべてのパラメータをエンジンに反映する */
  const applyParamsToEngine = useCallback((p: AllParams) => {
    const e = engineRef.current;
    if (!e) return;
    e.setOscillatorParams(p.oscillator);
    e.setNoiseParams(p.noise);
    e.setFilterParams(p.filter);
    e.setEnvelopeParams(p.envelope);
    e.setFilterEnvelopeParams(p.filterEnvelope);
    e.setLFOParams(p.lfo);
    e.setDriveParams(p.drive);
    e.setDelayParams(p.delay);
    e.setReverbParams(p.reverb);
    e.setUnisonParams(p.unison);
    e.setPerformanceParams(p.performance);
    e.setMasterVolume(p.masterVolume);
  }, []);

  /** プリセットを適用する。未指定フィールドはデフォルト値で埋める */
  const handleSelectPreset = useCallback(
    (preset: SynthPreset) => {
      const next: AllParams = {
        oscillator: preset.oscillator,
        noise: preset.noise ?? DEFAULT_NOISE,
        filter: preset.filter,
        envelope: preset.envelope,
        filterEnvelope: preset.filterEnvelope ?? DEFAULT_FILTER_ENVELOPE,
        lfo: preset.lfo,
        drive: preset.drive ?? DEFAULT_DRIVE,
        delay: preset.delay ?? DEFAULT_DELAY,
        reverb: preset.reverb ?? DEFAULT_REVERB,
        unison: preset.unison ?? DEFAULT_UNISON,
        performance: preset.performance ?? DEFAULT_PERFORMANCE,
        masterVolume: preset.masterVolume ?? DEFAULT_MASTER_VOLUME,
      };
      setParams(next);
      setPresetVersion((v) => v + 1);
      applyParamsToEngine(next);
    },
    [applyParamsToEngine],
  );

  /** すべて初期値に戻す */
  const handleReset = useCallback(() => {
    setParams(ALL_DEFAULTS);
    setPresetVersion((v) => v + 1);
    applyParamsToEngine(ALL_DEFAULTS);
  }, [applyParamsToEngine]);

  if (!engine) {
    return (
      <div className="app">
        <header className="app-header">
          <h1>シンセサイザー学習アプリ</h1>
        </header>
        <div style={startScreenStyle}>
          <button onClick={handleStart} style={startButtonStyle}>
            クリックして音声を有効化
          </button>
          <div style={hintStyle}>
            ブラウザの仕様上、最初の操作後に音声機能が利用可能になります。
          </div>
          {initError && (
            <div style={errorStyle}>音声初期化に失敗: {initError}</div>
          )}
        </div>
      </div>
    );
  }

  // key suffix。これを変えると Section が再マウントされ initialParams で再初期化
  const v = presetVersion;

  return (
    <SynthEngineProvider engine={engine}>
      <div className="app">
        <header className="app-header">
          <h1>シンセサイザー学習アプリ</h1>
          <PresetBrowser onSelect={handleSelectPreset} onReset={handleReset} />
        </header>

        <section className="visualizer-row">
          <WaveformView onParamHover={setHoveredParam} />
          <SpectrumView onParamHover={setHoveredParam} />
        </section>

        <section className="sections-row">
          <OscillatorSection
            key={`osc-${v}`}
            initialParams={params.oscillator}
            onParamHover={setHoveredParam}
          />
          <NoiseSection
            key={`noise-${v}`}
            initialParams={params.noise}
            onParamHover={setHoveredParam}
          />
          <FilterSection
            key={`flt-${v}`}
            initialParams={params.filter}
            onParamHover={setHoveredParam}
          />
          <FilterEnvelopeSection
            key={`fenv-${v}`}
            initialParams={params.filterEnvelope}
            onParamHover={setHoveredParam}
          />
          <EnvelopeSection
            key={`env-${v}`}
            initialParams={params.envelope}
            onParamHover={setHoveredParam}
          />
          <LFOSection
            key={`lfo-${v}`}
            initialParams={params.lfo}
            onParamHover={setHoveredParam}
          />
          <DriveSection
            key={`drv-${v}`}
            initialParams={params.drive}
            onParamHover={setHoveredParam}
          />
          <DelaySection
            key={`dly-${v}`}
            initialParams={params.delay}
            onParamHover={setHoveredParam}
          />
          <ReverbSection
            key={`rev-${v}`}
            initialParams={params.reverb}
            onParamHover={setHoveredParam}
          />
          <VoiceSection
            key={`voc-${v}`}
            initialUnison={params.unison}
            initialPerformance={params.performance}
            onParamHover={setHoveredParam}
          />
          <MasterSection
            key={`mst-${v}`}
            initialVolume={params.masterVolume}
            onParamHover={setHoveredParam}
          />
        </section>

        <Keyboard />

        <InfoPanel paramId={hoveredParam} />
      </div>
    </SynthEngineProvider>
  );
}

const startScreenStyle = {
  display: 'flex',
  flexDirection: 'column' as const,
  alignItems: 'center',
  justifyContent: 'center',
  gap: '16px',
  padding: '80px 16px',
};

const startButtonStyle = {
  background: 'var(--color-accent)',
  color: '#1a1a1a',
  border: 'none',
  padding: '14px 28px',
  fontSize: '15px',
  fontWeight: 600,
  borderRadius: '4px',
  cursor: 'pointer',
  letterSpacing: '0.05em',
};

const hintStyle = {
  color: 'var(--color-text-dim)',
  fontSize: '12px',
};

const errorStyle = {
  color: '#ff6464',
  fontSize: '13px',
  marginTop: '8px',
};
