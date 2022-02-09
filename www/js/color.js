function updateColor (isBrightness) {
  let sel = document.getElementById("selectedColor");

  let r = document.getElementById("r").value;
  let g = document.getElementById("g").value;
  let b = document.getElementById("b").value;

  sel.style.backgroundColor = "rgb("+r+","+g+","+b+")";
}

function updateAPI () {
  let r = document.getElementById("r").value;
  let g = document.getElementById("g").value;
  let b = document.getElementById("b").value;
  var xhr = new XMLHttpRequest();
  xhr.withCredentials = true;

  xhr.addEventListener("readystatechange", function() {
    if(this.readyState === 4) {
      console.log(this.responseText);
    }
  });

  xhr.open("POST", "./api?r="+r+"&g="+g+"&b="+b);
  xhr.send();
}

function getCurrentColor () {
  var xhr = new XMLHttpRequest();
  xhr.withCredentials = true;

  xhr.addEventListener("readystatechange", function() {
    if(this.readyState === 4) {
      console.log(this.responseText);
    }
  });

  xhr.open("POST", "./api");
  xhr.send();
}
