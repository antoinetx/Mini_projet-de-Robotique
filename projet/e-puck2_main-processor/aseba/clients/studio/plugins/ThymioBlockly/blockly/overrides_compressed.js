// Do not edit this file; automatically generated by build.py.
'use strict';

Blockly.Variables.flyoutCategory=function(m,k,g,c){if(Blockly.Blocks.thymio_declare_array){var b=Blockly.Block.obtain(c,"thymio_declare_array");b.initSvg();m.push(b);k.push(2*g)}b=Blockly.Variables.allVariables(c.targetWorkspace);b.sort(goog.string.caseInsensitiveCompare);b.unshift(null);for(var n=void 0,e=0;e<b.length;e++)if(b[e]!==n){var f=Blockly.Blocks.thymio_variable_get?Blockly.Block.obtain(c,"thymio_variable_get"):null;f&&f.initSvg();var d=Blockly.Blocks.thymio_variable_set?Blockly.Block.obtain(c,
"thymio_variable_set"):null;d&&d.initSvg();var l=Blockly.Blocks.thymio_get_array?Blockly.Block.obtain(c,"thymio_get_array"):null;l&&l.initSvg();var h=Blockly.Blocks.thymio_set_array?Blockly.Block.obtain(c,"thymio_set_array"):null;h&&h.initSvg();var a=Blockly.Blocks.math_number?Blockly.Block.obtain(c,"math_number"):null;a&&(a.setShadow(!0),a.initSvg(),d.inputList[0].connection.connect(a.outputConnection),a.render());if(a=Blockly.Blocks.math_number?Blockly.Block.obtain(c,"math_number"):null)a.setShadow(!0),
a.initSvg(),l.inputList[0].connection.connect(a.outputConnection),a.setFieldValue("0","NUM"),a.render();if(a=Blockly.Blocks.math_number?Blockly.Block.obtain(c,"math_number"):null)a.setShadow(!0),a.initSvg(),h.inputList[0].connection.connect(a.outputConnection),a.setFieldValue("0","NUM"),a.render();if(a=Blockly.Blocks.math_number?Blockly.Block.obtain(c,"math_number"):null)a.setShadow(!0),a.initSvg(),h.inputList[1].connection.connect(a.outputConnection),a.render();null===b[e]?n=(f||d||l||h).getVars()[0]:
(f&&f.setFieldValue(b[e],"VAR"),d&&d.setFieldValue(b[e],"VAR"),l&&l.setFieldValue(b[e],"VAR"),h&&h.setFieldValue(b[e],"VAR"));d&&m.push(d);f&&m.push(f);h&&m.push(h);l&&m.push(l);f&&d&&l&&h?k.push(g,g,g,3*g):k.push(2*g)}};Blockly.Blocks.logic.HUE=200;Blockly.Blocks.math.HUE=240;Blockly.Procedures.flyoutCategory=function(m,k,g,c){if(Blockly.Blocks.thymio_subroutine_define){var b=Blockly.Block.obtain(c,"thymio_subroutine_define");b.initSvg();m.push(b);k.push(2*g)}k.length&&(k[k.length-1]=3*g);for(var b=Blockly.Procedures.allProcedures(c.targetWorkspace)[0],n=0;n<b.length;n++){var e=Blockly.Block.obtain(c,"procedures_callnoreturn");e.setFieldValue(b[n][0],"NAME");for(var f=[],d=0;d<b[n][1].length;d++)f[d]="ARG"+d;e.setProcedureParameters(b[n][1],f);e.initSvg();m.push(e);k.push(2*
g)}};