open Note;
open SynthState;

type stateChange =
  | Voice(voice)
  | CurrentNoteChanged(note);

type listener = stateChange => unit;

type state = {
  currentNote: note,
  updateIndex: int,
  listeners: list(stateChange => unit),
  lastUpdate: list(stateChange),
};

let initialState: state = {
  currentNote: middleC,
  updateIndex: 0,
  listeners: [],
  lastUpdate: [],
};

type event =
  | RegisterListener(listener)
  | UnregisterListener(listener);

type acceptEvent = event => unit;

let emit = (state: state, stateChange: stateChange) => {
  Belt.List.forEach(state.listeners, listener => listener(stateChange));
};

let updateState = (prevState: state, event: event): state => {
  let state = {...prevState, updateIndex: prevState.updateIndex + 1, lastUpdate: []};

  let newState: state =
    switch (event) {
    | RegisterListener(listener) => {...state, listeners: [listener, ...state.listeners]}
    | UnregisterListener(listener) => {
        ...state,
        listeners: Belt.List.keep(state.listeners, registered => registered !== listener),
      }
    };

  Belt.List.forEach(newState.lastUpdate, emit(state));

  // Js.log(Printf.sprintf("RelativeNotesState transition to %s on %s", stringOfState(newState), stringOfEvent(event)));

  newState;
};

let stateRef: ref(state) = ref(initialState);

let dispatch = (event: event): unit => {
  stateRef := updateState(stateRef^, event);
};

// To help wrapping in react effects
let listenerEffect = (listener: stateChange => unit, dispatch: acceptEvent, _): option(unit => unit) => {
  dispatch(RegisterListener(listener));
  Some(() => dispatch(UnregisterListener(listener)));
};