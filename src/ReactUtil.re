type reactComponent2 =
  ReasonReact.componentSpec(
    ReasonReact.stateless,
    ReasonReact.stateless,
    ReasonReact.noRetainedProps,
    ReasonReact.noRetainedProps,
    ReasonReact.actionless,
  );

type reactComponent = ReasonReact.reactElement;

let asReactWithKey = (key: string, elementList: list(reactComponent)): ReasonReact.reactElement =>
  <React.Fragment key={key}>{ReasonReact.array(Belt.List.toArray(elementList))}</React.Fragment>;

let asReact = (elementList: list(reactComponent)): ReasonReact.reactElement =>
  ReasonReact.array(Belt.List.toArray(elementList));

let emptyFragment: reactComponent = asReact([])

let emptyFragmentWithKey = (key: string): reactComponent => <React.Fragment key = {"emptyFragment_" ++ key}>{emptyFragment}</React.Fragment>;
