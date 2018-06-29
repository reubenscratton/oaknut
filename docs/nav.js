
var navmodel = [
    { t: "Home", u:"/" },
    { t: "Guides",
    i: [
        { t: "Principles", u:"/guides/principles" },
        { t: "Build system" },
        { t: "Rendering" },
        { t: "Layout" },
        { t: "Styling" },
      ]
    },
    { t: "Samples",
      i: [
        { t: "Todo List" },
        { t: "Minesweeper" },
        { t: "BBC Micro", u:"/samples/embeeb/" },
      ],
    },
    { t: "API Reference",
      i: navmodelRef
    }
];

function toggleTocNode(node) {
  var toggle = node.getElementsByClassName("tocToggle")[0];
  var isOpen = toggle.classList.contains("tocToggleOpen");
  var children = node.getElementsByClassName("tocNode");
  for (var i=0 ; i<children.length ; i++) {
    var item = children[i];
    item.style.display = isOpen ? "none" : "block";
  }
  toggle.classList.remove(isOpen ? "tocToggleOpen" : "tocToggleClosed");
  toggle.classList.add(isOpen ? "tocToggleClosed" : "tocToggleOpen");
}

function addTocNodes(parentDiv, items) {
  items.forEach(function(item) {
      var itemDiv = document.createElement("div");
      itemDiv.classList.add('tocNode');

      var toggle = document.createElement("span");
      toggle.classList.add('tocToggle');
      itemDiv.appendChild(toggle);

      var link = document.createElement("A");
      if (item.hasOwnProperty("u")) {
        link.href = siteroot + item.u.substring(1);
      } else {
        link.href =  "#";
      }
      link.innerHTML = item.t;
      itemDiv.appendChild(link);

      parentDiv.appendChild(itemDiv);

      if (item.hasOwnProperty("i")) {
        if (item.i.length > 0) {
          toggle.classList.add('tocToggleOpen');
          toggle.addEventListener("click", (e) => {
            toggleTocNode(itemDiv);
            e.stopPropagation();
            e.preventDefault();
          });
          addTocNodes(itemDiv, item.i);
        }
      }
  });
}
