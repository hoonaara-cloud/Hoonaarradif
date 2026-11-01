# Hoonaar Audio — Radif Atlas (prototype 0.1)

An English-language educational VST3 reference plug-in for Iranian dastgah and avaz pitch material. It is designed for REAPER on Windows.

## What it does

- Select one of the 7 commonly listed dastgahs or 5 avaz.
- Select a tonic and enharmonic spelling.
- Browse a working catalogue of **247 gusheh names**.
- View a transposed seven-note reference scale on a five-line treble staff.
- See the scale's explicit accidentals, including sharp, flat, Koron and Sori symbols, plus note names and quarter-step/cents offsets.
- See Shahid and Ist where a source-specific degree is encoded. Unknown values are intentionally shown as “Not encoded” rather than invented.
- The audio path is transparent: this is a visual guide, not a sound generator, tuner, MIDI reader or pitch detector.

## Important musical scope

The included pitch templates use a 24-EDO teaching model (50 cents per quarter-step), transcribed into a custom seven-note scale signature. This is a **reference approximation**, not a claim that every Persian performance uses equal 50-cent Koron/Sori intervals. In practice, intonation, scale spelling, gusheh content, and the functions of Shahid and Ist can depend on the radif, teacher, instrument, and performance. Gushehs can also introduce altered notes or modulation, so a parent-scale display is not a complete melodic transcription of every gusheh.

The 247-name catalog is an editorial working list assembled from published radif listings. It is not a claim that there is one universally fixed list of exactly 250 gushehs. Some names and ordering differ between editions and performers. Only Shahid/Ist values supported by the working reference layer are entered; blank values should be completed against the exact edition selected for the final teaching product.

Before commercial release, have an Iranian classical-music teacher or musicologist validate the transcriptions and functions against the chosen printed/audio radif. See `DATA_NOTES.md`.

## Build the Windows VST3

This workspace contains the **source project**, not a precompiled `.vst3` binary. Build on Windows x64:

1. Install Visual Studio 2022 with **Desktop development with C++**.
2. Install CMake 3.22 or newer and Git; make sure both are available in PowerShell's `PATH`.
3. Open PowerShell in this folder. The first build needs internet access so CMake can fetch JUCE 9.0.0.
4. Run:

   ```powershell
   .\Build-Windows.ps1
   ```

   To build and copy the VST3 into the common system VST3 folder, run PowerShell as Administrator and use:

   ```powershell
   .\Build-Windows.ps1 -Install
   ```

The build script targets the Visual Studio 2022 x64 generator. The expected output is under `build\HoonaarRadifAtlas_artefacts\Release\VST3\`.

## Build with GitHub Actions

The repository includes `.github/workflows/build-windows.yml`. After this file is committed and pushed to GitHub, the workflow builds on push/PR and can also be started manually from **Actions → Build Windows VST3 → Run workflow**. Download `Radif-Atlas-Windows-VST3` from the run's **Artifacts** section; it contains the `.vst3` bundle in a ZIP. If the workflow does not appear, confirm the file is on the repository's default branch and Actions are enabled under **Settings → Actions → General**.

## Use in REAPER

1. Install or copy `Radif Atlas.vst3` to `C:\Program Files\Common Files\VST3\` (or the VST3 path REAPER scans).
2. In REAPER, open **Options → Preferences → Plug-ins → VST** and re-scan.
3. Add **Radif Atlas** as an FX on a track. It passes audio through unchanged and can be opened as a teaching/reference window.

## Project layout

- `Data/radif_data.json` — shared mode, pitch-template, gusheh-name and source data.
- `Source/` — JUCE VST3 source.
- `preview/radif_atlas_preview.html` — self-contained interactive browser preview (English UI).
- `Build-Windows.ps1` — Windows build/copy helper.
- `DATA_NOTES.md` — source and validation notes.

## License and third-party framework

The plugin source and original interface code in this project are provided for evaluation and further development. JUCE is fetched at build time under its own licensing terms; review the JUCE license before commercial distribution. The catalog and theory notes are attributed in `DATA_NOTES.md`.
