package doxypp;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Node;
import org.xml.sax.SAXException;

class CppClass {

	String name;
	String brief = "";
	String detailedDesc = "";
	List<String> baseclasses = new ArrayList<>();
	List<CppClassSection> sections = new ArrayList<>();
	
	
	CppClass(String path) throws ParserConfigurationException, SAXException, IOException {
		Xml xml = new Xml(path);		
		Node compounddef = Xml.n(xml.root, "compounddef");
		name = Xml.nt(compounddef, "compoundname");
		List<Node> basecompoundRefs = Xml.ns(compounddef, "basecompoundref");
		List<Node> sectionDefs = Xml.ns(compounddef, "sectiondef");
		for (Node basecompoundRef : basecompoundRefs) {
			baseclasses.add(basecompoundRef.getTextContent());
		}
		for (Node sectionDef : sectionDefs) {
			CppClassSection section = new CppClassSection(sectionDef);
			if (section.kind.equals("friend")) continue; // not interesting
			sections.add(section);
		}
		Node briefNode = Xml.n(compounddef, "briefdescription");
		if (briefNode != null) {
			brief = briefNode.getTextContent();
		}
		Node detailedDescNode = Xml.n(compounddef, "detaileddescription");
		if (detailedDescNode != null) {
			detailedDesc = detailedDescNode.getTextContent();
		}
	}
	
	String toMarkdown() {
		String s = "# " + name + "\n\n";
		s += "```\n"
		  +  "class " + name + "\n";
		if (baseclasses.size() > 0) {
			s += "    : ";
			for (int i=0 ; i<baseclasses.size() ; i++) {
				s += "public " + baseclasses.get(i) + ((i==(baseclasses.size()-1)) ? "" : ", ") + "\n";
			}
			s += "```\n\n";
		}
		if (brief.length() > 0) {
			s += brief + "\n";
		}
		if (detailedDesc.length() > 0) {
			s += detailedDesc + "\n";
		}
		for (CppClassSection section : sections) {
			s += section.toMarkdown();
		}
		s += "# Methods\n\n";
		for (CppClassSection section : sections) {
			for (CppMethod method : section.methods) {
				s += "| *" + method.name + "* | ";
				s += " `" + method.getSignatureHtml() + "` | ";
				s += "" + method.detailed + " |\n";
			}
		}
		return s;
	}
	String toHtml() {
		String s = "<h1>" + name + "</h1>\n\n";
		s += "<code>\n"
		  +  "class " + name + "\n";
		if (baseclasses.size() > 0) {
			s += "    : ";
			for (int i=0 ; i<baseclasses.size() ; i++) {
				s += "public " + baseclasses.get(i) + ((i==(baseclasses.size()-1)) ? "" : ", ");
			}
		}
		s += "</code>\n\n";
		if (brief.length() > 0) {
			s += "<p>" + brief + "</p>\n";
		}
		if (detailedDesc.length() > 0) {
			s += "<p>" + detailedDesc + "</p>\n";
		}
		for (CppClassSection section : sections) {
			s += section.toHtml();
		}
		s += "<h1>Methods</h1>\n\n";
		for (CppClassSection section : sections) {
			for (CppMethod method : section.methods) {
				s += "<h3>" + method.name + "</h3>\n";
				s += "<code>" + method.getSignatureHtml() + "</code>";
				s += "<p>" + method.detailed + "</p>";
				s += "\n\n";
			}
		}
		return s;
	}
}
