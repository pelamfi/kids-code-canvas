open Belt

type timerMs = {
  start: float,
  last: float,
};

type msDuration = {value: float}

/// Time from 0.0 to 1.0. Used for some definite piece of animation.
type unitT = {value: float}

type width = {value: float}

type velocity = {value: float}

type position = {value: float}

let lerp = (v0: float, v1: float, t: unitT): float => (1.0 -. t.value) *. v0 +. v1 *. t.value

let lerpWidth = (a: width, b: width, t: unitT): width => {value: lerp(a.value, b.value, t)}

let unitTClamp = (t: unitT): unitT => {value: Js.Math.min_float(Js.Math.max_float(0.0, t.value), 1.0)}

let tOfDuration = (fromStart: msDuration, total: msDuration): unitT => {
  {value: fromStart.value /. total.value}
}

let unitTCompressTowardsEnd = (original: unitT, compression: unitT): unitT => {
  {value: original.value *. (1.0 -. compression.value) +. compression.value}
}

let timerElapsed = (timer: option(timerMs)): msDuration => {
  switch (timer) {
  | Some(timer) => {value: timer.last -. timer.start}
  | None => {value: 0.0}
  };
};

let stringOfTimer = (timer: option(timerMs)): string => {
  Option.mapWithDefault(timer, "-", timer =>
    "(" ++ Js.Float.toString(timer.start) ++ " - " ++ Js.Float.toString(timer.last) ++ " ms)"
  );
};

let timerUpdated = (timer: option(timerMs), newLast: msDuration): option(timerMs) => {
  switch (timer) {
  | Some(timer) => Some({start: timer.start, last: Js.Math.max_float(timer.start, newLast.value)})
  | None => Some({start: newLast.value, last: newLast.value})
  };
};

let timerRestart = (timer: option(timerMs)): option(timerMs) => {
  switch (timer) {
  | Some(timer) => Some({start: timer.last, last: timer.last})
  | None => None
  };
};
