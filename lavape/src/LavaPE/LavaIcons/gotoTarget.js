function gotoTarget (linkTarget,divId) {
  var elm = document.getElementById(divId), parent = elm;

  while (parent) {
    if (parent.nodeName == "DIV")
		  parent.style.display = "block";
    parent = parent.parentNode;
  }
  window.location.href = linkTarget;
}