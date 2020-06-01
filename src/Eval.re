
// State which is evaluated by user provided scriplet and then rendered every frame
[@bs.deriving jsConverter]
type evalState = {
  x: float,
  y: float,
  r: float,
  g: float,
  b: float 
};

let evalScripletPrefix: string = {|
"use strict";
var x = 0.0;
var y = 0.0;
var punainen = 0.9;
var vihrea = 0.7;
var sininen = 0.4;
var lapinakyvyys = 1.0;
|};

let evalScripletSuffix: string = {|
return {x, y, r: punainen, g: vihrea, b: sininen};
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

let initial = createEvalFunction("x = t * 0.1 + (cos(t*9)+cos(t*2)) * 0.5; y = (sin(t*9)+sin(t*2)) * 0.5; sininen = cos(t * 3); vihrea = 0.5; punainen = sin(t)");

let initialState = createEvalFunction("")(0.0);

