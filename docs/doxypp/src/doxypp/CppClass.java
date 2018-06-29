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
	List<CppClassSection> sections = new ArrayList<>();
	
	
	CppClass(String path) throws ParserConfigurationException, SAXException, IOException {
		Xml xml = new Xml(path);		
		Node compounddef = Xml.n(xml.root, "compounddef");
		name = Xml.nt(compounddef, "compoundname");
		List<Node> sectionDefs = Xml.ns(compounddef, "sectiondef");
		for (Node sectionDef : sectionDefs) {
			CppClassSection section = new CppClassSection(sectionDef);
			if (section.kind.equals("friend")) continue; // not interesting
			sections.add(section);
		}
	}
	
	String toMarkdown() {
		String s = "# " + name + "\n\n";
		s += "```\n"
		  +  "class " + name + "\n"
		  +  "    : public Todo\n"
		  +	 "```\n\n";
		if (brief.length() > 0) {
			s += brief + "\n";
		}
		for (CppClassSection section : sections) {
			s += section.toMarkdown();
		}
		return s;
	}
	String toHtml() {
		String s = "<h1>" + name + "</h1>\n\n";
		s += "<code>\n"
		  +  "class " + name + "\n"
		  +  "    : public Todo\n"
		  +	 "</code>\n\n";
		if (brief.length() > 0) {
			s += "<p>" + brief + "</p>\n";
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
