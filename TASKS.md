# TASKS — 並行開発タスク一覧

5セッションでの並行開発用タスク定義。各セッションは1タスクをクレームし、
worktree 上で完結させ、main にマージする。

---

## 並行作業の原則

1. **触ってよいファイル** に明記されたパスのみを編集する。
2. **触ってはいけないファイル**（共有資産）は変更しない。変更が必要なら本ドキュメントの「I/F 変更ルール」へ。
3. 受け入れ条件をすべて満たしてから main にマージする。
4. ブランチ名は `session-{A|B|C|D|E}/{短い説明}` 形式。
5. 完了したら本ファイルの該当タスクのチェックボックスを更新し、コミット。

---

## 共有資産（全セッション共通・変更注意）

これらは「契約」です。勝手に変更しない:

- `src/types.ts` — 全モジュールの型定義
- `src/data/defaults.ts` — 各パラメータのデフォルト値
- `src/audio/engineContext.tsx` — React Context（Provider/Hook）
- `src/App.tsx` — ルートレイアウト（全セクション統合点）
- `src/main.tsx`
- `package.json`, `tsconfig.json`, `vite.config.ts`
- `TASKS.md`, `README.md`

### I/F 変更ルール

`src/types.ts` の変更が必要になった場合:
1. 作業を一時停止する。
2. main に「I/F変更提案」コミットを切る。
3. 他セッションへ周知（このファイルの「変更履歴」セクションに追記）。
4. 全セッションが pull した上で再開する。

---

## タスク A: 音声エンジン

- [x] 担当中: session-A/audio-engine
- [x] 完了

### 触ってよいファイル
- `src/audio/SynthEngine.ts`
- `src/audio/Oscillator.ts`
- `src/audio/Filter.ts`
- `src/audio/Envelope.ts`
- `src/audio/LFO.ts`
- 新規 `src/audio/__tests__/*` （任意）

### 触ってはいけないファイル
- `src/audio/engineContext.tsx`
- 上記「共有資産」全て

### 実装内容
1. `SynthEngine` クラスを `ISynthEngine` I/F に従って実装。
2. 信号フロー: Oscillator → Filter (Biquad) → VCA (Gain) → Master → AnalyserNode → Destination
3. ボイス管理: 簡易ポリ（最大4ボイス程度でOK、モノでも可）。
4. ADSR エンベロープを VCA の Gain に適用（`setValueAtTime` / `linearRampToValueAtTime`）。
5. LFO は OscillatorNode + Gain で生成し、ターゲット AudioParam に connect。
6. AnalyserNode は Master の前段に接続し、`getAnalyserNode()` で返す。

### 受け入れ条件
- `npm run typecheck` がパスする。
- `new SynthEngine()` → `init()` → `noteOn(60)` で音が鳴る（手動確認）。
- Filter の cutoff を変えると聴感上音色が変化する。
- ADSR のリリースが効く（noteOff 後にゆるやかにフェードアウト）。

---

## タスク B: UIプリミティブ（Knob + 共通スタイル）

- [x] 担当中: session-B/knob
- [x] 完了

### 触ってよいファイル
- `src/components/Knob.tsx`
- `src/styles/global.css`
- 新規 `src/styles/*.css`
- 新規 `src/components/__tests__/Knob*`（任意）

### 触ってはいけないファイル
- `src/components/sections/*`（Session D の領域）
- 上記「共有資産」全て

### 実装内容
1. `Knob` を `KnobProps` I/F に従って実装。
2. マウス縦ドラッグで値を増減（上で増加、下で減少）。
3. Shift+ドラッグでファイン操作（変化量 1/10）。
4. ダブルクリックでデフォルト値にリセット（オプション）。
5. `logarithmic: true` のとき対数スケールで値を割り当てる（周波数用）。
6. 回転角度で現在値を可視化（インジケータ線）。
7. ハードシンセ風スタイル: 黒/濃灰パネル、オレンジ系インジケータ。

### 受け入れ条件
- ドラッグで値が変化、`onChange` が連続発火する。
- ホバーで `onHover(paramId)` が発火、リーブで `onHover(null)`。
- 対数スケール（log）で 20Hz〜20000Hz を綺麗に補間できる。
- 既存の App.tsx でスタブ版と差し替えても壊れない。

---

## タスク C: ビジュアライザ

- [x] 担当中: session-C
- [x] 完了

### 触ってよいファイル
- `src/components/WaveformView.tsx`
- `src/components/SpectrumView.tsx`

### 触ってはいけないファイル
- 上記「共有資産」全て

### 実装内容
1. `useSynthEngine()` で engine を取得（null の場合はプレースホルダ表示）。
2. `engine.getAnalyserNode()` から `AnalyserNode` を取得。
3. WaveformView: `getByteTimeDomainData()` で時間波形を Canvas に描画。
4. SpectrumView: `getByteFrequencyData()` で周波数スペクトラムを Canvas に描画（対数軸推奨）。
5. `requestAnimationFrame` ループ、アンマウント時に停止。
6. Canvas は親要素にフィット（ResizeObserver で対応 or CSS で 100% 幅）。

### 受け入れ条件
- 音が鳴っているときに波形/スペクトラムが動く。
- 描画は 30fps 以上、メインスレッドをブロックしない。
- アンマウント時に rAF をクリーンアップ（メモリリークなし）。
- Engine が null のときは「未接続」表示で落ちない。

---

## タスク D: セクション + 解説パネル統合

- [ ] 担当中: _未割当_
- [ ] 完了

### 触ってよいファイル
- `src/components/sections/OscillatorSection.tsx`
- `src/components/sections/FilterSection.tsx`
- `src/components/sections/EnvelopeSection.tsx`
- `src/components/sections/LFOSection.tsx`
- `src/components/InfoPanel.tsx`

### 触ってはいけないファイル
- `src/components/Knob.tsx`（Session B の領域。Knob は I/F 経由でのみ使用）
- 上記「共有資産」全て

### 実装内容
1. 各セクションで `useSynthEngine()` から engine を取得。
2. ローカル state で現在のパラメータ値を保持し、変更時に engine.setXxxParams() を呼ぶ。
3. デフォルト値は `src/data/defaults.ts` から取得。
4. Knob には `paramId` をユニークに設定（例: `"filter.cutoff"`）。
5. 波形・フィルター種別・LFOターゲット等は ラジオボタン または セレクトで実装。
6. InfoPanel は与えられた paramId で `descriptions` を検索し、見つかれば表示。

### 受け入れ条件
- 各つまみを操作すると即座に engine 側のパラメータが更新される（型エラーなし）。
- つまみホバーで InfoPanel に該当解説が表示される。
- engine が null（Provider 外）でも UI は壊れない。

---

## タスク E: 鍵盤 + データ（解説 + プリセット）

- [ ] 担当中: _未割当_
- [ ] 完了

### 触ってよいファイル
- `src/components/Keyboard.tsx`
- `src/data/presets.ts`
- `src/data/descriptions.ts`

### 触ってはいけないファイル
- 上記「共有資産」全て

### 実装内容

**Keyboard:**
1. 1〜2オクターブ（C4〜C5 推奨）の鍵盤を SVG または div で描画。
2. マウスダウンで noteOn、マウスアップ/リーブで noteOff。
3. PCキーボード対応（A,W,S,E,D,F,T,G,Y,H,U,J,K の標準割当）。
4. 押下中の鍵をハイライト。
5. `useSynthEngine()` で engine を取得、null なら何もしない（落ちないこと）。

**Descriptions:**
- 全パラメータについて「数値の意味」と「人間の感覚での意味」を併記。
- 対象: oscillator.waveform, oscillator.detune, filter.type, filter.cutoff, filter.resonance,
  envelope.attack/decay/sustain/release, lfo.rate/depth/target/waveform, master.volume

**Presets:**
- 最低 5 種類: Basic Lead, Wobble Bass, Pad, Pluck, Stab など。

### 受け入れ条件
- 鍵盤クリックで音が鳴り、リリースで止まる。
- PCキーボードで演奏可能。
- 全パラメータに descriptions が用意され、InfoPanel に表示できる。
- presets が5件以上ある。

---

## 統合・最終確認（main で実施、最後に1人が担当）

- [ ] 全タスク完了後、main で `npm install && npm run dev` でアプリを起動。
- [ ] 鳴る・つまみで変化する・波形が動く・解説が出る、を確認。
- [ ] 軽微な統合修正（プロパティ受け渡しなど）を main で実施。

---

## 変更履歴（I/F 変更があったらここに追記）

- 2026-06-14 初期 I/F 確定（src/types.ts 初版）
