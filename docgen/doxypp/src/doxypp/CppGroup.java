package doxypp;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Node;
import org.xml.sax.SAXException;

public class CppGroup {
	
	/*
<doxygen>
  <compounddef id="group__views" kind="group">
    <compoundname>views</compoundname>
    <title>View System</title>
    <innerclass refid="class_view" prot="public">View</innerclass>
    <innerclass refid="class_window" prot="public">Window</innerclass>
    <briefdescription>
<para>Views, Windows, blah blah blah. </para>    </briefdescription>
    <detaileddescription>
    </detaileddescription>
  </compounddef>
</doxygen>
	 */
	
	public String name = "";
	public String title = "";
	public String briefDescription = "";
	public String detailedDescription = "";
	public List<CppClass> classes = new ArrayList<>();
	
	CppGroup(Node node) throws ParserConfigurationException, SAXException, IOException {
		Node compoundDef = Xml.n(node, "compounddef");
		name = Xml.nt(compoundDef, "compoundname");
		title = Xml.nt(compoundDef, "title");
		briefDescription = Xml.nt(compoundDef, "briefdescription");
		detailedDescription = Xml.nt(compoundDef, "detaileddescription");
		List<Node> innerClasses = Xml.ns(compoundDef, "innerclass");
		for (Node classNode : innerClasses) {
			Node classRef = classNode.getAttributes().getNamedItem("refid");
			CppClass cppNode = new CppClass(Doxy.inputPath + classRef.getTextContent() + ".xml");
			classes.add(cppNode);
		}
		
	}
	
	public String toHtml() {
		String s = "<h1>" + title +"</h1>\n\n";
		if (briefDescription.length() > 0) {
			s += "<p>" + briefDescription + "</p>\n\n";
		}
		if (detailedDescription.length() > 0) {
			s += "<p>" + detailedDescription + "</p>\n\n";			
		}
		s += "<h2>Classes</h2>\n\n";
		s += "<table>\n";
		for (CppClass clazz : classes) {
			s += "<tr><td>class <a href=\"" + clazz.name + ".html\">" +  clazz.name + "</a></td><td>" + clazz.brief + "</td></tr>\n";
		}
		s += "</table>\n";
		return s;
		
	}
	
	public String toMarkdown() {
		String s = "# " + title +"\n\n";
		if (briefDescription.length() > 0) {
			s += briefDescription + "\n\n";
		}
		if (detailedDescription.length() > 0) {
			s += detailedDescription + "\n\n";			
		}
		s += "## Classes\n\n";
		for (CppClass clazz : classes) {
			s += "`class ` [" +  clazz.name + "](todo) | " + clazz.brief + "\n";
		}
		return s;
	}
}
