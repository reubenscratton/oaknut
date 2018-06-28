package doxy2md;

import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class Doxy {
	
	static String inputPath = "doxygen/xml/";

	
	static void writeTextFile(String path, String markdown) throws IOException {
		FileOutputStream o = new FileOutputStream(path);		
		o.write(markdown.getBytes());
		o.close();		
	}
	static void writeHtmlFile(String title, String path, String html) throws IOException {
		html = "---\n" + 
			   "title: " + title + "\n" + 
			   "layout: default\n" + 
			   "---\n\n"
			   + html;
		writeTextFile(path, html);		
	}

	public static void main(String[] args) throws ParserConfigurationException, SAXException, IOException {
	
		// Find the group files
		File dir = new File(inputPath);
		String[] groupFiles = dir.list(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String name) {
				return name.startsWith("group_");
			}			
		});
		
		// Iterate the group files, convert each one to intermediate representation, and then emit markdown
		ArrayList<CppGroup> groups = new ArrayList<>();
		for (int i=0 ; i<groupFiles.length ; i++) {
			Xml xml = new Xml(inputPath + groupFiles[i]);			
			CppGroup group = new CppGroup(xml.root);
			groups.add(group);
			new File("ref/" + group.name).mkdirs();
			writeHtmlFile(group.title, "ref/" + group.name + "/index.html", group.toHtml());
			//writeTextFile("ref/" + group.name + "/index.md", group.toMarkdown());
			for (CppClass clazz : group.classes) {
				writeHtmlFile(clazz.name, "ref/" + group.name + "/" + clazz.name + ".html", clazz.toHtml());
				//writeTextFile("ref/" + group.name + "/" + clazz.name + ".md", clazz.toMarkdown());				
			}
		}
		
		Collections.sort(groups, new Comparator<CppGroup>() {
			@Override
			public int compare(CppGroup o1, CppGroup o2) {
				return o1.name.compareTo(o2.name);
			}			
		});
		
		// Build navmodel.js
		String navModel = "var navmodelRef = [\n";
		for (CppGroup group : groups) {
			navModel += "{ t:\"" + group.title + "\", i: [";
			for (CppClass clazz : group.classes) {
				navModel += "  { t:\"" + clazz.name + "\", u:\"/ref/" + group.name + "/" + clazz.name + "\"}";
				if (clazz != group.classes.get(group.classes.size()-1)) {
					navModel += ", \n";
				}
			}
			navModel += "]}";
			if (group != groups.get(groups.size()-1)) {
				navModel += ", \n";
			}
		}
		navModel += "]";
		writeTextFile("ref/navmodel.js", navModel);
	
	
	}

}