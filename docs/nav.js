
var navmodel = [
    { t: "Home", u:"/" },
    { t: "Guides",
    i: [
        { t: "Principles", u:"/guides/1_principles" },
        { t: "Building", u:"/guides/2_building"  },
        { t: "Threading", u:"/guides/3_threading" },
        { t: "Resources", u:"/guides/4_resources" },
        { t: "Styles", u:"/guides/5_styles" },
        { t: "Layout", u:"/guides/6_layout" },
        { t: "Rendering", u:"/guides/7_rendering" },
      ]
    },
    { t: "Samples",
      i: [
        { t: "Hello World", u:"/samples/helloworld/"  },
        { t: "Minesweeper", u:"/samples/minesweeper/" },
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
        link.href = siteroot + item.u;
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
