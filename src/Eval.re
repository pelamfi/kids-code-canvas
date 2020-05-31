
// State which is evaluated by user provided scriplet and then rendered every frame
[@bs.deriving jsConverter]
type evalState = {
  x: float,
  y: float
};

let evalScripletPrefix: string = {|
"use strict";
var x = 0.0;
var y = 0.0;
|};

let evalScripletSuffix: string = {|
return {x, y};
|};

type evalFunction = (float) => evalState;

let createEvalFunctionJs: (string) => evalFunction = [%bs.raw
  {|
function createEvalFunction(scriptlet) {
    var compiled = new Function("t", scriptlet);
    return (t) => {
        var result = compiled(t);
        var asRecord = evalStateFromJs(result);
        return asRecord;
    }
}
|}
];

let createEvalFunction = (scriplet: string): evalFunction => {
  let combinedScript = evalScripletPrefix ++ scriplet ++ evalScripletSuffix;
  createEvalFunctionJs(combinedScript);
};
