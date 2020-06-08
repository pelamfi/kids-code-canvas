
// State which is evaluated by user provided scriplet and then rendered every frame
[@bs.deriving jsConverter]
type scriptletState = {
  x: float,
  y: float,
  r: float,
  g: float,
  b: float,
  alpha: float,
  radius: float
};

let scripletCompilePrefix: string = {js|
"use strict";
var x = 0.0;
var y = 0.0;
var punainen = 0.0;
var vihreä = 0.0;
var sininen = 0.0;
var läpinäkyvyys = 0.0;
var koko = 0.1;
|js};

let dcripletCompileSuffix: string = {js|
;
return {x, y, r: punainen, g: vihreä, b: sininen, radius: koko, alpha: 1 - läpinäkyvyys};
|js};

type scriptletFunction = (float) => scriptletState;

let compileScriptletJs: (string) => scriptletFunction = [%bs.raw
  {|
function compileScriptletJs(scriptlet) {
    var errorFunc = (t) => scriptletStateFromJs({x: 0, y: 0, r: 0.9, g: 0.3, b: 0.3, radius: 0.05, alpha: 1});
    try {  
      var compiled = new Function("t", scriptlet.toLowerCase());
      return (t) => {
          try {  
            var result = compiled(t);
            var asRecord = scriptletStateFromJs(result);
            return asRecord;
          } catch (exception) {
            console.log("EVALUATION ERROR" + exception);
            return errorFunc;
          }          
      }
    } catch (exception) {
      console.log("COMPILE ERROR: " + exception);
      return errorFunc;
    }
}
|}
];

let compileScriptlet = (scriplet: string): scriptletFunction => {
  let scriptletWithMacros = scriplet 
    |> Js.String.replaceByRe([%re "/ on /g"], " = ")
    |> Js.String.replaceByRe([%re "/ plus /g"], " + ")
    |> Js.String.replaceByRe([%re "/ miinus /g"], " - ")
    |> Js.String.replaceByRe([%re "/ jaettuna /g"], " / ")
    |> Js.String.replaceByRe([%re "/ kertaa /g"], " * ")
    ;
  let combinedScript = scripletCompilePrefix ++ scriptletWithMacros ++ dcripletCompileSuffix;
  compileScriptletJs(combinedScript);
};

let initial = compileScriptlet("x = t");

let initialState = initial(0.0);