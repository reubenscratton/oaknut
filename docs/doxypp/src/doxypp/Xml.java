package doxypp;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public class Xml {
	
	Document doc;
	Element root;
	

	Xml(String path) throws ParserConfigurationException, SAXException, IOException {
		
	    File file = new File(path);
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
	    factory.setIgnoringElementContentWhitespace(true);
	    DocumentBuilder builder = factory.newDocumentBuilder();
	    doc = builder.parse(file);
		doc.getDocumentElement().normalize();

		root = doc.getDocumentElement();
	}
	
	static Node n(Node parent, String name) {
		NodeList nodes = parent.getChildNodes();
		for (int i=0 ; i<nodes.getLength() ; i++) {
			Node node = nodes.item(i);
			if (node.getNodeName() == name) {
				return node;
			}
		}
		return null;
	}
	static String nt(Node parent, String name) {
		Node n = n(parent, name);
		return (n == null) ? "" : n.getTextContent().trim();
	}
	static ArrayList<Node> ns(Node parent, String name) {
		ArrayList<Node> result = new ArrayList<Node>();
		NodeList nodes = parent.getChildNodes();
		for (int i=0 ; i<nodes.getLength() ; i++) {
			Node node = nodes.item(i);
			if (node.getNodeName() == name) {
				result.add(node);
			}
		}
		return result;
	}
	static String a(Node node, String attrName) {
		return node.getAttributes().getNamedItem(attrName).getTextContent();		
	}
	static boolean abool(Node node, String attrName) {
		String b = a(node, attrName);
		if (b.equals("no")) return false;
		if (b.equals("yes")) return true;
		throw new RuntimeException("Expected a bool value");
	}
}
