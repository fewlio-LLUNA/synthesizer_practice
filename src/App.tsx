import { useState } from 'react';
import { OscillatorSection } from './components/sections/OscillatorSection';
import { FilterSection } from './components/sections/FilterSection';
import { EnvelopeSection } from './components/sections/EnvelopeSection';
import { LFOSection } from './components/sections/LFOSection';
import { Keyboard } from './components/Keyboard';
import { WaveformView } from './components/WaveformView';
import { SpectrumView } from './components/SpectrumView';
import { InfoPanel } from './components/InfoPanel';

// アプリのルート。各セクションの統合点。
// 実装担当は Session D（セクション統合）と各セッションが分担。
export default function App() {
  const [hoveredParam, setHoveredParam] = useState<string | null>(null);

  return (
    <div className="app">
      <header className="app-header">
        <h1>シンセサイザー学習アプリ</h1>
      </header>

      <section className="visualizer-row">
        <WaveformView />
        <SpectrumView />
      </section>

      <section className="sections-row">
        <OscillatorSection onParamHover={setHoveredParam} />
        <FilterSection onParamHover={setHoveredParam} />
        <EnvelopeSection onParamHover={setHoveredParam} />
        <LFOSection onParamHover={setHoveredParam} />
      </section>

      <InfoPanel paramId={hoveredParam} />

      <Keyboard />
    </div>
  );
}
