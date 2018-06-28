package doxy2md;

import org.w3c.dom.Node;

class CppMember {

	CppType type;
	String name;
	String brief;
	String detailed;
	boolean isStatic;
	boolean isConst;
	String prot;
	String virt;

	CppMember(Node node) {
		name = Xml.nt(node,  "name");
		prot = Xml.a(node, "prot");
		isStatic = Xml.abool(node,  "static");
		isConst = Xml.abool(node,  "const");
		virt = Xml.a(node,  "virt");
		type = new CppType(Xml.n(node, "type"));
		brief = Xml.nt(node,  "briefdescription");
		detailed = Xml.nt(node,  "detaileddescription");

	}
}
