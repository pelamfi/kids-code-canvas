// https://www.wikiwand.com/en/Major_scale
// whole, whole, half, whole, whole, whole, half
// +2, +2, +1, +2 +2 +2 +1
// chromatic circle
// https://upload.wikimedia.org/wikipedia/commons/thumb/2/2a/Major_scale_in_the_chromatic_circle.png/440px-Major_scale_in_the_chromatic_circle.png
type noteInCMajor =
  | C
  | D
  | E
  | F
  | G
  | A
  | B; // H in finnish notation..

// I think chromatic scale is the one where all 12 notes are present.
// So I guess chromaticNote is somewhat correct name for this.
type chromaticNote =
  | C
  | CSharpDFlat
  | D
  | DSharpEFlat
  | E
  | F
  | FSharpGFlat
  | G
  | GSharpAFlat
  | A
  | ASharpBFlat
  | B;

type sharpOrFlat =
  | Flat // single letter note + flat
  | Sharp; // single letter note + sharp

// One note can have many names, depending on context.
// It is here expressed as the note in C major plus possible half step flat or sharp.
type noteName =
  | C
  | CSharpDFlat(sharpOrFlat)
  | D
  | DSharpEFlat(sharpOrFlat)
  | E
  | F
  | FSharpGFlat(sharpOrFlat)
  | G
  | GSharpAFlat(sharpOrFlat)
  | A
  | ASharpBFlat(sharpOrFlat)
  | B;

type noteNameClass =
  | Flat // single letter note - flat
  | Sharp // single letter note + sharp
  | Natural; // one of the "single letter notes" ie notes that make up CMajor

type noteHalfStep =
  | Flat // -1 half step
  | Natural // 0
  | Sharp; // +1 half step https://www.wikiwand.com/en/Sharp_(music)

// https://bucklescript.github.io/docs/en/generate-converters-accessors#convert-between-jst-object-and-record
[@bs.deriving jsConverter]
type note = {
  // https://www.wikiwand.com/en/Piano_key_frequencies
  // offset from middle C (which is then denoted as 0 here)
  offset: int,
};

type scaleClass =
  | Chromatic
  | Major
  | Minor;

type scale =
  | Chromatic
  | Major(note)
  | Minor(note);

type scaleName = {
  noteName,
  scaleClass,
};

type interval = {steps: int};

// https://en.wikipedia.org/wiki/Octave?oldformat=true#Octave_of_a_pitch

// The number is in the scientific octave naming system. 0 is the octave where A is 27.5
// https://www.wikiwand.com/en/Musical_note#/section_Note_designation_in_accordance_with_octave_name
// https://www.wikiwand.com/en/Scientific_pitch_notation
type octave = {number: int};

let middleCOctave: octave = {number: 4}; // aka one lined

let octaveOfNote = (note: note): octave => {
  let shiftedBy0Octave = note.offset + middleCOctave.number * 12;
  {number: MathUtil.flooredDivision(shiftedBy0Octave, 12)};
};

let subscriptOfOctave = (octave: octave): string => {
  switch (octave.number) {
  | 0 => {js|₀|js}
  | 1 => {js|₁|js}
  | 2 => {js|₂|js}
  | 3 => {js|₃|js}
  | 4 => {js|₄|js}
  | 5 => {js|₅|js}
  | 6 => {js|₆|js}
  | 7 => {js|₇|js}
  | 8 => {js|₈|js}
  | 9 => {js|₉|js}
  | _ => ""
  };
};

let middleC: note = {offset: 0};

let cMajorName: scaleName = {noteName: C, scaleClass: Major};

let oneLinedNoteOfNote = (n: note): note => {
  {offset: MathUtil.flooredDivisionRemainder(n.offset, 12)};
};

let oneLinedNoteOfChromaticNote = (n: chromaticNote): note => {
  switch (n) {
  | C => {offset: 0}
  | CSharpDFlat => {offset: 1}
  | D => {offset: 2}
  | DSharpEFlat => {offset: 3}
  | E => {offset: 4}
  | F => {offset: 5}
  | FSharpGFlat => {offset: 6}
  | G => {offset: 7}
  | GSharpAFlat => {offset: 8}
  | A => {offset: 9}
  | ASharpBFlat => {offset: 10}
  | B => {offset: 11}
  };
};

let noteOfOctaveAndChromaticNote = (o: octave, n: chromaticNote): note => {
  {offset: (o.number - middleCOctave.number) * 12 + oneLinedNoteOfChromaticNote(n).offset};
};

let asChromaticNote = (n: note): chromaticNote => {
  switch (oneLinedNoteOfNote(n).offset) {
  | 0 => C
  | 1 => CSharpDFlat
  | 2 => D
  | 3 => DSharpEFlat
  | 4 => E
  | 5 => F
  | 6 => FSharpGFlat
  | 7 => G
  | 8 => GSharpAFlat
  | 9 => A
  | 10 => ASharpBFlat
  | _ => B
  };
};

// https://www.wikiwand.com/en/Chromatic_scale
// In descending notes, flat notation would be used, but I'm not interested
// in adding ascending/descending to this model yet..
let chromaticNoteNames: list(noteName) = [
  C,
  CSharpDFlat(Sharp),
  D,
  DSharpEFlat(Sharp),
  E,
  F,
  FSharpGFlat(Sharp),
  G,
  GSharpAFlat(Sharp),
  A,
  ASharpBFlat(Sharp),
  B,
];

let range_of_int = (base: note, start: int, rangeEnd: int): list(note) => {
  RangeOfInt.make(start, rangeEnd) |> RangeOfInt.map(x => {offset: base.offset + x});
};

let inverse = (i: interval): interval => {steps: i.steps * (-1)};

let chromaticIntervals = Array.init(12, i => {steps: i}) |> Array.to_list;

let scaleIntervals = (s: scaleClass): list(interval) => {
  switch (s) {
  | Chromatic => chromaticIntervals
  | Major => [{steps: 0}, {steps: 2}, {steps: 3}] // TODO: Correct steps
  | Minor => [{steps: 0}, {steps: 1}]
  };
};

let noteApplyInterval = (n: note, i: interval): note => {offset: n.offset + i.steps};

let scale = (rootNote: note, s: scaleClass): list(note) => {
  let intervals = scaleIntervals(s);
  Belt.List.map(intervals, interval => noteApplyInterval(rootNote, interval));
};

let asNoteNameClass = (n: sharpOrFlat): noteNameClass => {
  switch (n) {
  | Sharp => Sharp
  | Flat => Flat
  };
};

let asNoteNameClass = (n: noteName): noteNameClass => {
  switch (n) {
  | DSharpEFlat(sharpOrFlatValue)
  | CSharpDFlat(sharpOrFlatValue)
  | FSharpGFlat(sharpOrFlatValue)
  | GSharpAFlat(sharpOrFlatValue)
  | ASharpBFlat(sharpOrFlatValue) => asNoteNameClass(sharpOrFlatValue)
  | _ => Natural
  };
};

// If you have a chromatic note and a flat or sharp scale, the
// scale sharp/flat decides wheter the note is denoted flat or sharp.
let asNoteName = (n: chromaticNote, c: noteNameClass): noteName => {
  let sharpOrFlatValue: sharpOrFlat =
    switch (c) {
    | Flat => Flat
    | Natural => Sharp // ?
    | Sharp => Sharp
    };

  switch (n) {
  | C => C
  | CSharpDFlat => CSharpDFlat(sharpOrFlatValue)
  | D => D
  | DSharpEFlat => DSharpEFlat(sharpOrFlatValue)
  | E => E
  | F => F
  | FSharpGFlat => FSharpGFlat(sharpOrFlatValue)
  | G => G
  | GSharpAFlat => GSharpAFlat(sharpOrFlatValue)
  | A => A
  | ASharpBFlat => ASharpBFlat(sharpOrFlatValue)
  | B => B
  };
};

// If you know a note and a scale, you know what that note is called
let noteNameForScaleName = (n: note, scale: scaleName): noteName => {
  let chromaticNote = asChromaticNote(n);
  let noteClass = asNoteNameClass(scale.noteName);
  asNoteName(chromaticNote, noteClass);
};

let stringOfNoteName = (n: noteName): string => {
  switch (n) {
  | C => "C"
  | CSharpDFlat(Sharp) => {js|C♯|js}
  | CSharpDFlat(Flat) => {js|D♭|js}
  | D => "D"
  | DSharpEFlat(Sharp) => {js|D♯|js}
  | DSharpEFlat(Flat) => {js|E♭|js}
  | E => "E"
  | F => "F"
  | FSharpGFlat(Sharp) => {js|F♯|js}
  | FSharpGFlat(Flat) => {js|G♭|js}
  | G => "G"
  | GSharpAFlat(Sharp) => {js|G♯|js}
  | GSharpAFlat(Flat) => {js|A♭|js}
  | A => "A"
  | ASharpBFlat(Sharp) => {js|A♯|js}
  | ASharpBFlat(Flat) => {js|B♭|js}
  | B => "B"
  };
};

let nameOfNoteInCMajor = (note: note): string => {
  stringOfNoteName(noteNameForScaleName(note, cMajorName)) ++ subscriptOfOctave(octaveOfNote(note));
};

let nameOfChromaticNote = (chromaticNote: chromaticNote): string => {
  let oneLinedNote = oneLinedNoteOfChromaticNote(chromaticNote)
  stringOfNoteName(noteNameForScaleName(oneLinedNote, cMajorName))
}

let min = (note: note, otherNote: note): note => {
  {offset: Js.Math.min_int(note.offset, otherNote.offset)};
};

let frequency = (n: note): float => {
  // middle C is the 0, so shift by 9 half tones to A which is the nice round 440 hz
  Js.Math.pow_float(~base=2.0, ~exp=float(n.offset - 9) /. 12.0) *. 440.0;
};

let nameOfInterval = (interval: interval): string => {
  switch (Js.Math.abs_int(interval.steps)) {
  | 0 => "Perfect unison"
  | 1 => "Minor second"
  | 2 => "Major second"
  | 3 => "Minor third"
  | 4 => "Major third"
  | 5 => "Perfect fourth"
  | 6 => "Dimished fifth"
  | 7 => "Perfect fifth"
  | 8 => "Minor sixth"
  | 9 => "Major sixth"
  | 10 => "Minor seventh"
  | 11 => "Major seventh"
  | 12 => "Perfect octave"
  | _ => ""
  };
};

let noteEq = (a: note, b: note): bool => a.offset == b.offset

// https://en.wikipedia.org/wiki/Octave?oldformat=true#Octave_of_a_pitch
let minOctave: octave = {number: 1}; // A 55 hz
let maxOctave: octave = {number: 6};
let minNote: note = noteOfOctaveAndChromaticNote(minOctave, C);
let maxNote: note = noteOfOctaveAndChromaticNote(maxOctave, B);
let numberOfNotes: int = maxNote.offset - minNote.offset + 1;

let loopOctaves = (n: note): note => {
  let loopedOffset = MathUtil.flooredDivisionRemainder(n.offset - minNote.offset, numberOfNotes) + minNote.offset;
  // Js.log(Printf.sprintf("n.offset:%d minNote.offset:%d numberOfNotes:%d loopedOffset: %d", n.offset, minNote.offset, numberOfNotes, loopedOffset));
  {offset: loopedOffset};
};
