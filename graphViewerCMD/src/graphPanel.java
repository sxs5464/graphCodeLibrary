/*
 * This file is part of the GraphCodeLibrary.
 * 
 * GraphCodeLibrary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * GraphCodeLibrary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GraphCodeLibrary.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * graphPanel.java
 *
 *  Created on: Feb 18, 2015
 *      Author: Sam Skalicky
 */

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.StringTokenizer;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;

import com.mxgraph.layout.hierarchical.mxHierarchicalLayout;
import com.mxgraph.swing.mxGraphComponent;
import com.mxgraph.util.mxCellRenderer;
import com.mxgraph.util.mxConstants;
import com.mxgraph.view.mxGraph;

public class graphPanel extends JPanel {

	mxGraphComponent graphComponent;
	mxGraph graph;

	public graphPanel() {
		graph = new mxGraph();
		graphComponent = new mxGraphComponent(graph);

		graphComponent.getViewport().setOpaque(true);
		graphComponent.getViewport().setBackground(Color.WHITE);

		add(graphComponent);
	}

	static class Types {
		static Integer Empty = 0;
		static Integer Add = 1;
		static Integer Mult = 2;
		static Integer Div = 3;
		static Integer Sub = 4;
		static Integer Mod = 5;

		private static Integer memOffset = 10;
		static Integer Mem0 = 0;
		static Integer Mem1 = memOffset;
		static Integer Mem2 = memOffset*2;
		static Integer Mem3 = memOffset*3;
		static Integer Mem4 = memOffset*4;

		public static int getMemType(Integer val) {
			if(val >= Mem4)
				return Mem4/memOffset;
			else if(val >= Mem3)
				return Mem3/memOffset;
			else if(val >= Mem2)
				return Mem2/memOffset;
			else if(val >= Mem1)
				return Mem1/memOffset;
			else
				return 0;
		}

		static int setMemType(Integer val) {
			return memOffset * val;
		}

		public static int getOpType(Integer val) {
			Integer tmp = val % Mem1;
			if(tmp == Empty)
				return Empty;
			else if(tmp == Add)
				return Add;
			else if(tmp == Mult)
				return Mult;
			else if(tmp == Div)
				return Div;
			else if(tmp == Sub)
				return Sub;
			else if(tmp == Mod)
				return Mod;
			else
				return -1;
		}
	}

	int[][] readMatrix(String filename) {
		int data[][] = null;

		File file = new File(filename);
		if(file.exists()) {		
			try {
				//open the file
				BufferedReader br = new BufferedReader(new FileReader(file));
				String line;

				//count the number of lines
				int cnt=0;
				while((line = br.readLine()) != null) {
					cnt++;
				}

				br = new BufferedReader(new FileReader(file));

				//create adjacency data structure
				data = new int[cnt][cnt];

				//read the adjacency matrix
				int i=0;
				while((line = br.readLine()) != null) {

					//tokenize on spaces
					StringTokenizer st = new StringTokenizer(line);

					int numTokens = st.countTokens();
					//loop over all tokens
					for(int j=0; j<numTokens; j++) {
						//get token
						String tok = st.nextToken();	

						//check for no connection
						if(tok.compareTo("-") == 0) {
							data[i][j] = -1;
						}
						else {
							//parse value
							data[i][j] = Integer.parseInt(tok);
						}						
					}
					i++;
				}			
			} catch (FileNotFoundException e) {e.printStackTrace();
			} catch (NumberFormatException e) {e.printStackTrace();
			} catch (IOException e) {e.printStackTrace();}
			
			return data;
		}
		else { 
			System.out.println("ERROR! File: " + filename + " not found.");
			return new int[1][1];
		}
	}

	public void load(String filename) {
		String ops[] = {"","+","\u00D7","\u00F7","-","%"};

		int data[][] = readMatrix(filename);

		graph.removeCells(graph.getChildVertices(graph.getDefaultParent()));
		Object[] vertices = new Object[data.length];

		//create vertices
		for(int i=0; i<data.length; i++) {
			vertices[i]=graph.insertVertex(null, null, (ops[Types.getOpType(data[i][i])]+(i)), 100+50*i, 100, 40, 40, mxConstants.STYLE_SHAPE+"="+mxConstants.SHAPE_ELLIPSE);
		}

		//create edges
		for(int i=0; i<data.length; i++) {
			for(int j=0; j<data.length; j++) {
				if(i != j && data[i][j] == 1)
					graph.insertEdge(null, null, "", vertices[i], vertices[j]);
			}
		}


		//arrange the nodes so the graph looks pretty 
		mxHierarchicalLayout layout = new mxHierarchicalLayout(graph);
		layout.execute(graph.getDefaultParent());
	}

	public void savePNG(String filename) {
		// Creates the image for the PNG file
		BufferedImage image = mxCellRenderer.createBufferedImage(graph,
				null, 1, Color.white, graphComponent.isAntiAlias(), null,
				graphComponent.getCanvas());

		try {
			ImageIO.write(image, "png", new File(filename));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void GUI(graphPanel panel) {
		JFrame frame = new JFrame();
		frame.getContentPane().add(panel);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(870, 640);
		frame.setVisible(true);
	}

	public static void main(String[] args) {
		if(args.length != 1) {
			System.out.println("usage: graphPanel <DFG.txt>");
		}
		else {
			graphPanel panel = new graphPanel();

			panel.load(args[0]);
			panel.savePNG(args[0].substring(0, args[0].length()-4) + ".png");
		}
	}

}
