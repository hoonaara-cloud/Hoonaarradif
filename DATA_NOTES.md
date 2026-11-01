# Data and notation notes

## Reference model

`Data/radif_data.json` contains 12 top-level families: seven commonly listed dastgahs and five avaz. The scale spellings are based on the mode list in the Foundation for Iranian Studies' *Persian Classical Music: System of Modes*. The list uses pitch spellings such as `Ap` for a pitch approximately a quarter-tone below A; these were converted here to quarter-step offsets in a 24-EDO teaching model.

Each `scale24` array contains seven offsets from the selected tonic. One unit is 50 cents in this prototype, and the displayed value is `scale24[i] × 50¢`. The pitch model can be transposed to any selected tonic. The `referenceTonic` and `referenceTonicName` fields use the source's underlined note as the default tonic where the page marks one; the Shur and Homayun defaults use the first note of the listed example. Pitch classes are C=0 through B=11.

This notation is deliberately described as a **custom scale signature** rather than a standard Western key signature. A seven-note scale with microtonal degrees does not always map cleanly to a conventional major/minor signature. The score therefore displays the degree-by-degree alterations and keeps the Koron/Sori indication explicit. Real intonation may not equal exactly 50 cents; tuning presets and cents-per-degree calibration are future work.

## Gusheh catalog

The working catalog contains 247 English-transliterated gusheh names across the 12 families. It is a useful navigation list, not a complete licensed transcription of a printed or recorded radif. Some entries are variants, repeated sections, rhythmic forms, or explicitly marked names seen in other radifs. A definitive commercial release should select one exact edition/ روایت, normalize its name spellings, then review the full ordered list with a domain expert.

The `shahid` and `ist` properties are one-based scale degrees. A `null` value means the project does not yet have a source-checked value for that gusheh. A value at degree 8 or above is displayed in the upper octave. These roles are melodic functions and can change by gusheh; they should not be inferred from tonic alone.

## Main references consulted

- Foundation for Iranian Studies, *Persian Classical Music: System of Modes* — the seven-dastgah/five-avaz grouping and representative scale spellings: <https://fis-iran.org/ebook/persian-system-of-modes/>
- *Radif (Iranian music)* — overview of radif scope and variation in counts/names/order: <https://fa.wikipedia.org/wiki/%D8%B1%D8%AF%DB%8C%D9%81_(%D9%85%D9%88%D8%B3%DB%8C%D9%82%DB%8C)
- Shur gusheh list: <https://tanbur-music.ir/%D8%AF%D8%B3%D8%AA%DA%AF%D8%A7%D9%87-%D9%85%D9%88%D8%B3%DB%8C%D9%82%DB%8C/%DA%AF%D9%88%D8%B4%D9%87%D9%87%D8%A7%DB%8C-%D8%AF%D8%B3%D8%AA%DA%AF%D8%A7%D9%87-%D8%B4%D9%88%D8%B1/>
- Chahargah, gusheh order and degree relationships: <https://fa.wikipedia.org/wiki/%D8%AF%D8%B3%D8%AA%DA%AF%D8%A7%D9%87_%DA%86%D9%87%D8%A7%D8%B1%DA%AF%D8%A7%D9%87>
- Mahur, gusheh list and notes on Delkash/Shekasteh: <https://fa.wikipedia.org/wiki/%D9%85%D8%A7%D9%87%D9%88%D8%B1_(%D8%AF%D8%B3%D8%AA%DA%AF%D8%A7%D9%87_%D9%85%D9%88%D8%B3%DB%8C%D9%82%D9%8A)
- Nava, gusheh names and common order: <https://sornaz.com/%DA%AF%D9%88%D8%B4%D9%87-%D9%87%D8%A7%DB%8C-%D8%AF%D8%B3%D8%AA%DA%AF%D8%A7%D9%87-%D9%86%D9%88%D8%A7/>
- Published Mirza Abdollah radif listing for oud, based on Daryoush Tala'i's book: <https://mahoor.com/fa/cd/20843-the-mirz%C3%A2-abdoll%C3%A2h-radif-for-ud-cd>
- Bayat-e Tork gusheh and function discussion: <https://choubsaz.ir/index.php/en/blog/151-2018-11-07-13-02-31>
- General note on how Shahid/Ist can be gusheh-specific, with Shur examples: <https://fa.wikipedia.org/wiki/%D8%B4%D9%88%D8%B1_(%D8%AF%D8%B3%D8%AA%DA%AF%D8%A7%D9%87_%D9%85%D9%88%D8%B3%DB%8C%D9%82%D9%8A)

## Before shipping a final version

1. Choose the exact source radif (for example, a named Mirza Abdollah transcription/ روایت) and the intended spelling/transliteration convention.
2. Review every gusheh name and ordering against that edition.
3. Encode each gusheh's actual pitch material, altered/variable degrees, modulation, Shahid, Ist, and cadence behavior—not only the parent seven-note template.
4. Decide on the target intonation model (24-EDO as a classroom convention, a measured cents table, or editable performer/teacher values).
5. Have a Persian classical-music teacher validate the English instructional wording and all notation before commercial release.
