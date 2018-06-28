package doxy2md;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

class CppType {

// <type><ref refid="class_view" kindref="compound">View</ref> *</type>
// <type>ObjPtr&lt; class <ref refid="class_view_controller" kindref="compound">ViewController</ref> &gt;</type>
	
//	String str;
String html;

	CppType(Node node) {
		//str = node.getTextContent(); // todo, parse properly
		NodeList list = node.getChildNodes();
		html = "";
	    for (int i = 0; i < list.getLength(); ++i) {
	        Node child = list.item(i);
	        if (child.getNodeType() == Node.TEXT_NODE) {
	        	html += child.getTextContent(); 
	        } else {
	        	if (child.getNodeName().equals("ref")) {
	        		String refid = Xml.a(child,  "refid");
		        	html += "<a href=\"" + refid + ".html\">" + child.getTextContent() + "</a>";        		
	        	} else {
	        		throw new RuntimeException("unexpected tag in type");
	        	}
	        }
	    }
	}	
	String html() {
		return html;
	}
	
}
