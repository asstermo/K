// Macro recorded on: Sun Sep 11 2011 19:34:26 GMT+0100 (GMT Standard Time)
komodo.assertMacroVersion(3);
if (komodo.view) { komodo.view.setFocus(); }

var beforeList=["¨",   "»", "ž",  "œ",  "Ð",  "¬", "„", "", ""];
var afterList=["REM ", "→", ">=", "<=", "|>", "º", "∆", "!=", "'"];

for(var i=0; i<beforeList.length;i++) {
    Find_ReplaceAllInMacro(window, 0, beforeList[i], afterList[i], false, 0, 0, false, false);
}