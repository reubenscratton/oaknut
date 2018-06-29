package doxypp;

import java.util.ArrayList;
import java.util.List;

import org.w3c.dom.Node;

class CppMethod extends CppMember {

	class Parameter {
		CppType type;
		String name;
	}
	boolean isVirtual;
	List<Parameter> parameters = new ArrayList<>();
	
	CppMethod(Node node) {
		super(node);		
		String virt = Xml.a(node, "virt");
		isVirtual = virt.equalsIgnoreCase("virtual");
		List<Node> params = Xml.ns(node,  "param");
		for (Node paramNode : params) {
			Parameter param = new Parameter();
			param.type = new CppType(Xml.n(paramNode, "type"));
			param.name = Xml.nt(paramNode, "declname");
			parameters.add(param);
		}

	}
	
	String getSignature() {
		String s = /*prot + " " +*/ "`" + type.html + " " + name + "(";
		for (Parameter param : parameters) {
			s += param.type.html + " " + param.name;
			if (param != parameters.get(parameters.size()-1)) {
				s += ", ";
			}
		}
		s += ")`";
		return s;
	}
	String getSignatureHtml() {
		String s = type.html() + " <a href=\"todo\">" + name + "</a>(";
		for (Parameter param : parameters) {
			s += param.type.html() + " " + param.name;
			if (param != parameters.get(parameters.size()-1)) {
				s += ", ";
			}
		}
		s += ")";
		return s;
	}
}
