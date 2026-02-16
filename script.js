document.getElementById("essayForm").addEventListener("submit", async function(e) {
  e.preventDefault(); // so no reload
  var name = document.getElementById("name").value;
  var essay = document.getElementById("essay").value;

  var res = await fetch("/", {
    method: "POST",
    headers: {"Content-Type": "application/json"},
    body: JSON.stringify({name: name, essay: essay})
  });

  var data = await res.json();
  if (data.error) {
    alert(data.error);
    return;
  }

  document.getElementById("grade").textContent = data.grade;
  var list = document.getElementById("reasons");
  list.innerHTML = "";
  if (data.reasons.length === 0) {
    list.innerHTML = "<li>yayyy!! 100 !</li>";
  } else {
    data.reasons.forEach(function(r) {
      // for each list item
      var li = document.createElement("li");
      li.textContent = r;
      list.appendChild(li);
    });
  }
  document.getElementById("result").style.display = "block";
});
