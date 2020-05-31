open Note;

type voiceState =
  | Idle
  | AttackRelease
  | Attack
  | Release;

type voiceKey =
  | Single(note);

type triggerId =
  | Idle;

type voice = {
  key: voiceKey,
  updateIndex: int,
  triggerId,
  state: voiceState,
  prevState: voiceState,
  allocated: int
};

let voices = 6;
