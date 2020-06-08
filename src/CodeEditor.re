[@react.component]
let make = (~compiledScriptlet: CodeCanvasState.compiledScriptlet) => {

  let onChange = (e: ReactEvent.Form.t): unit => {
    CodeCanvasState.dispatch(CodeCanvasState.ChangeScriptlet(e->ReactEvent.Form.target##value));
  };

  <div className="codeEditorCell">
    <textarea className="codeEditor" onChange=onChange rows=10 defaultValue=compiledScriptlet.scriptlet.scriptletString/>
  </div>
};