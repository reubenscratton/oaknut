package doxypp;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import java.util.HashMap;

class CppType {

	NodeList list;

	public static HashMap<String, String> refMap = new HashMap<>();

	CppType(Node node) {
		list = node.getChildNodes();
	}

	String toText() {

		String text = "";
		for (int i = 0; i < list.getLength(); ++i) {
			Node child = list.item(i);
			if (child.getNodeType() == Node.TEXT_NODE) {
				text += child.getTextContent();
			} else {
				if (child.getNodeName().equals("ref")) {
					String refid = Xml.a(child,  "refid");
					String href = CppType.refMap.get(refid);
					if (href != null) {
						//text += "<a href=\"" + href + "\">" + child.getTextContent() + "</a>";
						text += "`[`"+child.getTextContent() + "`]("+href+")`";

					} else {
						text += child.getTextContent();
					}
				} else {
					throw new RuntimeException("unexpected tag in type");
				}
			}
		}
		return text;
	}
	
}
