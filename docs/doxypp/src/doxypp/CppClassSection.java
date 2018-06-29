package doxypp;

import java.util.ArrayList;
import java.util.List;

import org.w3c.dom.Node;

class CppClassSection {

	String title;
	String kind;
	ArrayList<CppMethod> methods = new ArrayList<>();
	
	CppClassSection(Node node) {
		title = Xml.nt(node,  "header");
		kind = Xml.a(node, "kind");
		List<Node> memberDefs = Xml.ns(node, "memberdef");
		for (Node memberDef : memberDefs) {
			String memberKind = Xml.a(memberDef, "kind");
			if (memberKind.equals("public-function")
				|| memberKind.equals("function")) {
				CppMethod method = new CppMethod(memberDef);
				methods.add(method);
			}
		}
	}

	String toMarkdown() {
		String s = "## " + title + "\n\n";
		for (CppMethod method : methods) {
			s += method.getSignature() + " | ";
			if (method.brief.length() > 0) {
				s += method.brief;
			}
			s += "\n";
		}
		s += "\n\n";
		return s;
	}
	String toHtml() {
		String s = "<h2>" + title + "</h2>\n\n";
		s += "<table>\n";
		for (CppMethod method : methods) {
			s += "<tr><td>";
			s += method.getSignatureHtml() + "</td>";
			s += "<td>";
			if (method.brief.length() > 0) {
				s += method.brief;
			}
			s += "</td></tr>\n";
		}
		s += "<table>\n\n";
		return s;
	}
}
