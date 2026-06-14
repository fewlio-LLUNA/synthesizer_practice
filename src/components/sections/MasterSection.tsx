import { useState } from 'react';
import type { SectionProps } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_MASTER_VOLUME } from '../../data/defaults';
import { Knob } from '../Knob';

interface Props extends SectionProps {
  initialVolume?: number;
}

export function MasterSection({ onParamHover, initialVolume }: Props) {
  const engine = useSynthEngine();
  const [volume, setVolume] = useState(initialVolume ?? DEFAULT_MASTER_VOLUME);

  const handleVolumeChange = (value: number) => {
    setVolume(value);
    engine?.setMasterVolume(value);
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Master</h3>
      <Knob
        paramId="master.volume"
        label="Volume"
        value={volume}
        min={0}
        max={1}
        step={0.01}
        onChange={handleVolumeChange}
        onHover={onParamHover}
      />
    </div>
  );
}

const panelStyle: React.CSSProperties = {
  background: 'var(--color-panel)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '4px',
  padding: '12px',
  minHeight: '160px',
};

const titleStyle: React.CSSProperties = {
  margin: '0 0 8px 0',
  fontSize: '12px',
  fontWeight: 600,
  letterSpacing: '0.1em',
  color: 'var(--color-accent)',
};
