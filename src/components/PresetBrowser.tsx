import { useState } from 'react';
import type { SynthPreset } from '../types';
import { presets } from '../data/presets';

interface Props {
  onSelect: (preset: SynthPreset) => void;
  onReset: () => void;
}

export function PresetBrowser({ onSelect, onReset }: Props) {
  const [selectedName, setSelectedName] = useState<string>('');

  const handleSelectChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    const name = e.target.value;
    setSelectedName(name);
    const preset = presets.find((p) => p.name === name);
    if (preset) onSelect(preset);
  };

  const handleReset = () => {
    setSelectedName('');
    onReset();
  };

  const selectedPreset = presets.find((p) => p.name === selectedName);

  return (
    <div style={containerStyle}>
      <div style={rowStyle}>
        <label style={labelStyle}>Preset</label>
        <select
          value={selectedName}
          onChange={handleSelectChange}
          style={selectStyle}
        >
          <option value="">— 選択してください —</option>
          {presets.map((p) => (
            <option key={p.name} value={p.name}>
              {p.name}
            </option>
          ))}
        </select>
        <button type="button" onClick={handleReset} style={buttonStyle}>
          Reset
        </button>
      </div>
      {selectedPreset && (
        <div style={descStyle}>{selectedPreset.description}</div>
      )}
    </div>
  );
}

const containerStyle: React.CSSProperties = {
  display: 'flex',
  flexDirection: 'column',
  gap: '4px',
};

const rowStyle: React.CSSProperties = {
  display: 'flex',
  alignItems: 'center',
  gap: '8px',
};

const labelStyle: React.CSSProperties = {
  fontSize: '11px',
  color: 'var(--color-text-dim)',
  letterSpacing: '0.05em',
};

const selectStyle: React.CSSProperties = {
  background: 'var(--color-panel)',
  color: 'var(--color-text)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '3px',
  padding: '4px 8px',
  fontSize: '12px',
  minWidth: '160px',
};

const buttonStyle: React.CSSProperties = {
  background: 'var(--color-knob)',
  color: 'var(--color-text)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '3px',
  padding: '4px 12px',
  fontSize: '11px',
  cursor: 'pointer',
};

const descStyle: React.CSSProperties = {
  fontSize: '11px',
  color: 'var(--color-text-dim)',
  fontStyle: 'italic',
};
