
let sel;
let r;
let g;
let b;
let l;
let mutexLock = false;

function updateAPI (forced) {
  let sel = document.getElementById("selectedColor");
  let r = document.getElementById("r").value;
  let g = document.getElementById("g").value;
  let b = document.getElementById("b").value;
  let l = document.getElementById("l").value;

  sel.style.backgroundColor = "rgb("+r+","+g+","+b+")";
  if (!mutexLock || forced) {
    mutexLock = true;
    var xhr = new XMLHttpRequest();
    xhr.withCredentials = true;

    xhr.addEventListener("readystatechange", function() {
      if(this.readyState === 4) {
        mutexLock = false;
        console.log(this.responseText);
        let colors = this.responseText.split(",");
        r = colors[0];
        b = colors[1];
        g = colors[2];
        l = colors[3];
      }
    });

    xhr.open("POST", "./api?r="+r+"&g="+g+"&b="+b+"&l="+l);
    xhr.send();
  }
}

function getCurrentColor () {
  var xhr = new XMLHttpRequest();
  xhr.withCredentials = true;

  xhr.addEventListener("readystatechange", function() {
    if(this.readyState === 4) {
      console.log(this.responseText);
      let colors = this.responseText.split(",");
      r = colors[0];
      b = colors[1];
      g = colors[2];
      l = colors[3];
    }
  });

  xhr.open("POST", "./api");
  xhr.send();
}
