function toggleDisplay (secNum) {
  var elm = document.getElementById(secNum);
	if (elm.style.display == "none")
		{elm.style.display = "block"}
	else
		{elm.style.display = "none"}
}