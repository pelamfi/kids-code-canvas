type a;

let throttle = (func: 'a => unit, ~timeout = 1000): ('a => unit) => {
    let throttling = ref(false);
    (a: 'a) => {
        if (! throttling^) {
            throttling := true;
            func(a);
            Js.Global.setTimeout(() => throttling := false, timeout) |> ignore;
        }
    };
};

let debounce = (func: 'a => unit, ~timeout = 10000): ('a => unit) => {
    let timerHandle: ref(option(Js.Global.timeoutId)) = ref(None);
    (a: 'a) => {
        timerHandle^
          -> Belt.Option.mapWithDefault((), Js.Global.clearTimeout);
        timerHandle := Some(Js.Global.setTimeout(() => func(a), timeout));
    };
};