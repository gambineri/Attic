/***
 * CSS utilities
 ***/

//------------------------------------------------------------------------------
function setStyle(itm, newStyle) 
{
  if (itm.document.all) 
  {
	  if (itm.document.styleSheets == null)
	    return;

	  if (itm.document.styleSheets.length == null)
	    return;

	  for(i = 0; i < itm.document.styleSheets.length; i++) 
    {
	    for(j = 0; j < itm.document.styleSheets[i].rules.length; j++) 
      {
	      if (itm.document.styleSheets[i].rules[j] == null)
	        return;

	      if (itm.document.styleSheets[i].rules[j].selectorText == newStyle) 
        {
          setMandatoryStyleProperty(itm.style, "fontFamily", itm.document.styleSheets[i].rules[j].style.fontFamily);
          setStyleProperty(itm.style, "fontStyle", itm.document.styleSheets[i].rules[j].style.fontStyle);
          setStyleProperty(itm.style, "fontWeight", itm.document.styleSheets[i].rules[j].style.fontWeight);
          setStyleProperty(itm.style, "fontSize", itm.document.styleSheets[i].rules[j].style.fontSize);
          setMandatoryStyleProperty(itm.style, "font", itm.document.styleSheets[i].rules[j].style.font);
          setStyleProperty(itm.style, "color" , itm.document.styleSheets[i].rules[j].style.color);
          setStyleProperty(itm.style, "backgroundColor", itm.document.styleSheets[i].rules[j].style.backgroundColor);
          setStyleProperty(itm.style, "backgroundImage", itm.document.styleSheets[i].rules[j].style.backgroundImage);
          setStyleProperty(itm.style, "backgroundRepeat", itm.document.styleSheets[i].rules[j].style.backgroundRepeat);
          setStyleProperty(itm.style, "backgroundAttachement", itm.document.styleSheets[i].rules[j].style.backgroundAttachement);
          setStyleProperty(itm.style, "backgroundPosition", itm.document.styleSheets[i].rules[j].style.backgroundPosition);
          setStyleProperty(itm.style, "backgroundPositionX", itm.document.styleSheets[i].rules[j].style.backgroundPositionX);
          setStyleProperty(itm.style, "backgroundPositionY", itm.document.styleSheets[i].rules[j].style.backgroundPositionY);
          setStyleProperty(itm.style, "letterSpacing", itm.document.styleSheets[i].rules[j].style.letterSpacing);
          setStyleProperty(itm.style, "textDecoration", itm.document.styleSheets[i].rules[j].style.textDecoration);
          setStyleProperty(itm.style, "textDecorationNone", itm.document.styleSheets[i].rules[j].style.textDecorationNone);
          setStyleProperty(itm.style, "textDecorationUnderline", itm.document.styleSheets[i].rules[j].style.textDecorationUnderline);
          setStyleProperty(itm.style, "textDecorationOverLine", itm.document.styleSheets[i].rules[j].style.textDecorationOverline);
          setStyleProperty(itm.style, "textDecorationLineThrough", itm.document.styleSheets[i].rules[j].style.textDecorationLineThrough);
          setStyleProperty(itm.style, "textDecorationBlink", itm.document.styleSheets[i].rules[j].style.textDecorationBlink);
          setStyleProperty(itm.style, "verticalAlign", itm.document.styleSheets[i].rules[j].style.verticalAlign);
          setStyleProperty(itm.style, "textTransform", itm.document.styleSheets[i].rules[j].style.textTransform);
          setStyleProperty(itm.style, "textAlign", itm.document.styleSheets[i].rules[j].style.textAlign);
          setStyleProperty(itm.style, "textIndent", itm.document.styleSheets[i].rules[j].style.textIndent);
          setStyleProperty(itm.style, "lineHeight", itm.document.styleSheets[i].rules[j].style.lineHeight);
          setStyleProperty(itm.style, "marginTop", itm.document.styleSheets[i].rules[j].style.marginTop);
          setStyleProperty(itm.style, "marginRight", itm.document.styleSheets[i].rules[j].style.marginRight);
          setStyleProperty(itm.style, "marginBottom", itm.document.styleSheets[i].rules[j].style.marginBottom);
          setStyleProperty(itm.style, "marginLeft", itm.document.styleSheets[i].rules[j].style.marginLeft);
          setStyleProperty(itm.style, "margin", itm.document.styleSheets[i].rules[j].style.margin);
          setStyleProperty(itm.style, "paddingTop", itm.document.styleSheets[i].rules[j].style.paddingTop);
          setStyleProperty(itm.style, "paddingBottom", itm.document.styleSheets[i].rules[j].style.paddingBottom);
          setStyleProperty(itm.style, "paddingLeft", itm.document.styleSheets[i].rules[j].style.paddingLeft);
          setStyleProperty(itm.style, "paddingRight", itm.document.styleSheets[i].rules[j].style.paddingRight);
          setStyleProperty(itm.style, "padding", itm.document.styleSheets[i].rules[j].style.padding);
          setStyleProperty(itm.style, "border", itm.document.styleSheets[i].rules[j].style.border);
          setStyleProperty(itm.style, "borderColor", itm.document.styleSheets[i].rules[j].style.borderColor);
          setStyleProperty(itm.style, "borderStyle", itm.document.styleSheets[i].rules[j].style.borderStyle);
          setStyleProperty(itm.style, "borderWidth", itm.document.styleSheets[i].rules[j].style.borderWidth);
          setStyleProperty(itm.style, "borderTop", itm.document.styleSheets[i].rules[j].style.borderTop);
          setStyleProperty(itm.style, "borderTopColor", itm.document.styleSheets[i].rules[j].style.borderTopColor);
          setStyleProperty(itm.style, "borderTopStyle", itm.document.styleSheets[i].rules[j].style.borderTopStyle);
          setStyleProperty(itm.style, "borderTopWidth", itm.document.styleSheets[i].rules[j].style.borderTopWidth);
          setStyleProperty(itm.style, "borderBottom", itm.document.styleSheets[i].rules[j].style.borderBottom);
          setStyleProperty(itm.style, "borderBottomColor", itm.document.styleSheets[i].rules[j].style.borderBottomColor);
          setStyleProperty(itm.style, "borderBottomStyle", itm.document.styleSheets[i].rules[j].style.borderBottomStyle);
          setStyleProperty(itm.style, "borderBottomWidth", itm.document.styleSheets[i].rules[j].style.borderBottomWidth);
          setStyleProperty(itm.style, "borderLeft", itm.document.styleSheets[i].rules[j].style.borderLeft);
          setStyleProperty(itm.style, "borderLeftColor", itm.document.styleSheets[i].rules[j].style.borderLeftColor);
          setStyleProperty(itm.style, "borderLeftStyle", itm.document.styleSheets[i].rules[j].style.borderLeftStyle);
          setStyleProperty(itm.style, "borderLeftWidth", itm.document.styleSheets[i].rules[j].style.borderLeftWidth);
          setStyleProperty(itm.style, "borderRight", itm.document.styleSheets[i].rules[j].style.borderRight);
          setStyleProperty(itm.style, "borderRightColor", itm.document.styleSheets[i].rules[j].style.borderRightColor);
          setStyleProperty(itm.style, "borderRightStyle", itm.document.styleSheets[i].rules[j].style.borderRightStyle);
          setStyleProperty(itm.style, "borderRightWidth", itm.document.styleSheets[i].rules[j].style.borderRightWidth);
          setStyleProperty(itm.style, "clear", itm.document.styleSheets[i].rules[j].style.clear);
          setStyleProperty(itm.style, "display", itm.document.styleSheets[i].rules[j].style.display);
          setStyleProperty(itm.style, "visibility", itm.document.styleSheets[i].rules[j].style.visibility);
          setStyleProperty(itm.style, "cursor", itm.document.styleSheets[i].rules[j].style.cursor);                    
          setStyleProperty(itm.style, "listStyleType", itm.document.styleSheets[i].rules[j].style.listStylePosition);
          setStyleProperty(itm.style, "listStyleTypeImage", itm.document.styleSheets[i].rules[j].style.listStyleImage);
          setStyleProperty(itm.style, "listStyle", itm.document.styleSheets[i].rules[j].style.listStyle);
          setStyleProperty(itm.style, "listStylePosition", itm.document.styleSheets[i].rules[j].style.listStylePosition);
          setStyleProperty(itm.style, "top", itm.document.styleSheets[i].rules[j].style.top);
          setStyleProperty(itm.style, "left", itm.document.styleSheets[i].rules[j].style.left);
          setStyleProperty(itm.style, "width", itm.document.styleSheets[i].rules[j].style.width);
          setStyleProperty(itm.style, "height", itm.document.styleSheets[i].rules[j].style.height);
          setStyleProperty(itm.style, "bottom", itm.document.styleSheets[i].rules[j].style.bottom);
          setStyleProperty(itm.style, "right", itm.document.styleSheets[i].rules[j].style.right);
	      }
	    }
	  }
	}
}

//------------------------------------------------------------------------------
function setStyleProperty(styleObj, setProperty, valProperty) 
{
  if (valProperty != null) 
    eval("styleObj." + setProperty + " = valProperty;");
}

//------------------------------------------------------------------------------
function setMandatoryStyleProperty(styleObj, setProperty, valProperty) 
{
  if (valProperty != null) 
    if (valProperty.length > 0) 
      eval("styleObj." + setProperty + " = valProperty;");
}

//------------------------------------------------------------------------------
function showHints(td) 
{
	if (document.all != null) 
  {
  	showDiv(document.all['HintWindow']);
  	document.all['HintWindow'].style.width = td.offsetWidth + 4;
  	document.all['HintWindow'].style.left = td.offsetLeft;
  	document.all['HintWindow'].style.top = td.offsetTop + td.offsetHeight + 4;
  }
}    

//------------------------------------------------------------------------------
function hideHints(td) 
{
	if (document.all != null) 
  	hideDiv(document.all['HintWindow']);
}  

//------------------------------------------------------------------------------
function showSteps(td) 
{
  if (document.all != null) 
  {
	  showDiv(document.all['StepWindow']);
	  document.all['StepWindow'].style.width = td.offsetWidth + 4;
	  document.all['StepWindow'].style.left = td.offsetLeft;
	  document.all['StepWindow'].style.top = td.offsetTop + td.offsetHeight + 4;
	}
}  

//------------------------------------------------------------------------------
function hideSteps(td) 
{
	if (document.all != null) 
  	hideDiv(document.all['StepWindow']);
}  

//------------------------------------------------------------------------------
function hideDiv(div) 
{
  div.style.visibility='hidden';
}

//------------------------------------------------------------------------------
function showDiv(div) 
{
	div.style.visibility='visible';
}	
