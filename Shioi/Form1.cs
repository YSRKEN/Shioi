using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Diagnostics;

namespace Shioi {
	public partial class MainForm : Form {
		Renju FormRenju;
		const string SoftName = "Shioi";
		const int BoardSize = 15;
		const string PositionStringX = "abcdefghijklmno";
		const string PositionStringY = "123456789ABCDEF";
		public enum Stone {
			None,
			Black,
			White
		}

		public MainForm() {
			InitializeComponent();
			FormRenju = new Renju();
			DrawBoard();
		}

		private void NewToolStripMenuItem_Click(object sender, EventArgs e) {
			FormRenju = new Renju();
		}
		private void OpenToolStripMenuItem_Click(object sender, EventArgs e) {
			// Show "open file dialog"
			var ofd = new OpenFileDialog();
			ofd.Filter = "Gomoku Kyousou(*.ban)|*.ban|Date Gomoku(*.dt5)|*.dt5|Fiver6(*.lws)|*.lws|Renju Kozou(*.rnj)|*.rnj|MonteCarlo Gomokunarabe(*.kif)|*.kif|Niroi Renju hu(*.nrf)|*.nrf";
			if(ofd.ShowDialog() != DialogResult.OK)
				return;
			// Open select file
			FormRenju = new Renju(ofd.FileName);
			DrawBoard();
		}
		private void SaveToolStripMenuItem_Click(object sender, EventArgs e) {
			// Show "save file dialog"
			var sfd = new SaveFileDialog();
			sfd.Filter = "Gomoku Kyousou(*.ban)|*.ban|Date Gomoku(*.dt5)|*.dt5|Fiver6(*.lws)|*.lws|Renju Kozou(*.rnj)|*.rnj|MonteCarlo Gomokunarabe(*.kif)|*.kif|Niroi Renju hu(*.nrf)|*.nrf";
			if(sfd.ShowDialog() != DialogResult.OK)
				return;
			// Save select file
			FormRenju.SaveFile(sfd.FileName);
		}
		private void ExitToolStripMenuItem_Click(object sender, EventArgs e) {
			this.Close();
		}

		private void CopyMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			var output = FormRenju.ToStringMove();
			Clipboard.SetDataObject(output, true);
		}
		private void PasteMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			var data = Clipboard.GetDataObject();
			if(data.GetDataPresent(DataFormats.Text)) {
				var input = (string)data.GetData(DataFormats.Text);
				FormRenju.Parse(input);
				LastMoveToolStripMenuItem_Click(sender, e);
			}
		}
		private void CopyBoardTextToolStripMenuItem_Click(object sender, EventArgs e) {
			var output = FormRenju.ToStringBoard();
			Clipboard.SetDataObject(output, true);
		}
		private void CopyBoardPictureToolStripMenuItem_Click(object sender, EventArgs e) {
			var bmp = new Bitmap(PictureBox.Image);
			Clipboard.SetImage(bmp);
		}
		private void CopyBoardandTurnTextAToolStripMenuItem_Click(object sender, EventArgs e) {
			Clipboard.SetDataObject(FormRenju.GetArgumentString(), true);
		}

		private void ForwardToolStripMenuItem_Click(object sender, EventArgs e) {
			FormRenju.Next();
			DrawBoard();
		}
		private void BackwardToolStripMenuItem_Click(object sender, EventArgs e) {
			FormRenju.Prev();
			DrawBoard();
		}
		private void FirstMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			int pointer = FormRenju.MovePointer;
			for(int i = 0; i <= pointer; ++i)
				FormRenju.Prev();
			DrawBoard();
		}
		private void LastMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			int pointer = FormRenju.MovePointer;
			for(int i = pointer; i < FormRenju.Move.Count; ++i)
				FormRenju.Next();
			DrawBoard();
		}
		private void FirstMoveButton_Click(object sender, EventArgs e) {
			FirstMoveToolStripMenuItem_Click(sender, e);
		}
		private void BackwardButton_Click(object sender, EventArgs e) {
			BackwardToolStripMenuItem_Click(sender, e);
		}
		private void ForwardButton_Click(object sender, EventArgs e) {
			ForwardToolStripMenuItem_Click(sender, e);
		}
		private void LastMoveButton_Click(object sender, EventArgs e) {
			LastMoveToolStripMenuItem_Click(sender, e);
		}

		private void StartComputingToolStripMenuItem_Click(object sender, EventArgs e) {
			var psInfo = new ProcessStartInfo();
			psInfo.FileName = System.Windows.Forms.Application.StartupPath + @"\I401.exe";
			psInfo.CreateNoWindow = true;
			psInfo.UseShellExecute = false;
			psInfo.Arguments = FormRenju.GetArgumentString();
			psInfo.RedirectStandardOutput = true;
			psInfo.RedirectStandardError = true;
			this.Text = SoftName + " - Thinking - ";
			var sw = new System.Diagnostics.Stopwatch();
			sw.Start();
			Process p = Process.Start(psInfo);
			var output = p.StandardOutput.ReadToEnd();
			var output_error = p.StandardError.ReadToEnd();
			sw.Stop();
			this.Text = SoftName;
			ElapsedTimeStatusLabel.Text = "Time : " + sw.ElapsedMilliseconds.ToString() + "[ms]";
			var nextMove = int.Parse(output);
			if(nextMove >= 0) {
				FormRenju.SetMove(nextMove);
				DrawBoard();
			}
			if(output_error != "") {
				MessageBox.Show(output_error, SoftName);
			}
		}
		private void StopComputingToolStripMenuItem_Click(object sender, EventArgs e) {

		}
		private void StartComputingButton_Click(object sender, EventArgs e) {
			StartComputingToolStripMenuItem_Click(sender, e);
		}
		private void StopComputingButton_Click(object sender, EventArgs e) {
			StopComputingToolStripMenuItem_Click(sender, e);
		}
		private void ShowMoveNumberComboBox_SelectedIndexChanged(object sender, EventArgs e) {
			DrawBoard();
		}

		private void AboutAToolStripMenuItem_Click(object sender, EventArgs e) {
			var assem = Assembly.GetExecutingAssembly();
			var v = assem.GetName().Version;
			var version = v.Major.ToString() + "." + v.Minor.ToString() + "." + v.Build.ToString() + "." + v.Revision.ToString();
			var assemblyProduct = ((AssemblyProductAttribute)Attribute.GetCustomAttribute(assem, typeof(AssemblyProductAttribute))).Product;
			var assemblyCopyright = ((AssemblyCopyrightAttribute)Attribute.GetCustomAttribute(assem, typeof(AssemblyCopyrightAttribute))).Copyright;
			var verInfo = 
				"SoftName : " + assemblyProduct + "\n"
				+ "Version : " + version + "\n"
				+ "Created by YSR\n"
				+ assemblyCopyright;
			MessageBox.Show(verInfo, SoftName);
		}

		private void PictureBox_MouseClick(object sender, MouseEventArgs e) {
			var blockSize = PictureBox.Width / 16;
			var x = e.Location.X / blockSize - 1;
			var y = e.Location.Y / blockSize - 1;
			if(x < 0 || x >= BoardSize || y < 0 || y >= BoardSize)
				return;
			if(FormRenju.Point(x, y) != Stone.None)
				return;
			var p = Renju.ConvertMove2to1(x, y);
			FormRenju.SetMove(p);
			DrawBoard();
		}
		private void MainForm_DragDrop(object sender, DragEventArgs e) {
			string[] files = (string[])e.Data.GetData(DataFormats.FileDrop, false);
			if(files.Length < 1)
				return;
			if(Path.GetExtension(files[0]) != ".ban"
			&& Path.GetExtension(files[0]) != ".dt5"
			&& Path.GetExtension(files[0]) != ".lws"
			&& Path.GetExtension(files[0]) != ".rnj"
			&& Path.GetExtension(files[0]) != ".kif"
			&& Path.GetExtension(files[0]) != ".nrf")
				return;
			// Open select file
			FormRenju = new Renju(files[0]);
			DrawBoard();
		}
		private void MainForm_DragEnter(object sender, DragEventArgs e) {
			if(e.Data.GetDataPresent(DataFormats.FileDrop)) {
				e.Effect = DragDropEffects.All;
			} else {
				e.Effect = DragDropEffects.None;
			}
		}

		private void DrawBoard() {
		// Draw board
			var canvas = new Bitmap(PictureBox.Width, PictureBox.Height);
			var blockSize = PictureBox.Width / 16;
			var boardOffset = blockSize * 3 / 8;
			var g = Graphics.FromImage(canvas);
			// Draw back ground
			g.FillRectangle(Brushes.Gold, 0, 0, PictureBox.Width, PictureBox.Height);
			// Draw grid line
			for(int y = 1; y <= BoardSize; ++y) g.DrawLine(Pens.Black, blockSize + boardOffset, blockSize * y + boardOffset, blockSize * BoardSize + boardOffset, blockSize * y + boardOffset);
			for(int x = 1; x <= BoardSize; ++x) g.DrawLine(Pens.Black, blockSize * x + boardOffset, blockSize + boardOffset, blockSize * x + boardOffset, blockSize * BoardSize + boardOffset);
			// Draw position signature
			var fontSize = blockSize / 2;
			var font = new Font("MS Gothic", fontSize, FontStyle.Bold);
			for(int y = 0; y < BoardSize; ++y) g.DrawString(PositionStringY.Substring(y, 1), font, Brushes.Black, blockSize / 8, blockSize * y + blockSize * 3 / 4 + boardOffset);
			for(int x = 0; x < BoardSize; ++x) g.DrawString(PositionStringX.Substring(x, 1), font, Brushes.Black, blockSize * x + blockSize * 3 / 4 + boardOffset, blockSize / 8);
			// Draw star-mark
			const int StarR = 3;
			g.FillEllipse(Brushes.Black, blockSize *  4 - StarR + boardOffset, blockSize *  4 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize * 12 - StarR + boardOffset, blockSize *  4 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize * 12 - StarR + boardOffset, blockSize * 12 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize *  4 - StarR + boardOffset, blockSize * 12 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize *  8 - StarR + boardOffset, blockSize *  8 - StarR + boardOffset, StarR * 2, StarR * 2);
			// Draw stones
			var StoneR = blockSize / 2;
			var font2 = new Font("MS Gothic", fontSize - 2, FontStyle.Bold);
			var showNumbers = ShowMoveNumberComboBox.SelectedIndex - 1;
			for(int p = 0; p <= FormRenju.MovePointer; ++p) {
				// Calc parameter
				var MoveXY = Renju.ConvertMove1to2(FormRenju.Move[p]);
				var drawX = blockSize * MoveXY[0] + StoneR + 1 + boardOffset;
				var drawY = blockSize * MoveXY[1] + StoneR + 1 + boardOffset;
				var pLength = (p + 1).ToString().Length;
				var pLengthOffset = pLength * (fontSize - 2) / 2;
				int[] offsetX1 = {0, fontSize, fontSize * 19 / 16, fontSize * 5 / 4}; 
				var offsetX = - pLengthOffset + offsetX1[pLength];
				var offsetY = fontSize / 2;
				// Draw stone
				switch(FormRenju.Point(p)) {
				case Stone.Black:
					// Black stone
					g.FillEllipse(Brushes.Black, drawX, drawY, StoneR * 2, StoneR * 2);
					if(showNumbers < 0) {
						g.DrawString((p + 1).ToString(), font2, Brushes.White, drawX + offsetX, drawY + offsetY);
					}else {
						if(FormRenju.MovePointer - p < showNumbers) {
							var showNumber = p + showNumbers - FormRenju.MovePointer;
							var pLength2 = showNumber.ToString().Length;
							var pLengthOffset2 = pLength2 * (fontSize - 2) / 2;
							var offsetX2 = -pLengthOffset2 + offsetX1[pLength2];
							g.DrawString(showNumber.ToString(), font2, Brushes.White, drawX + offsetX2, drawY + offsetY);
						}
					}
					break;
				case Stone.White:
					// White stone
					g.FillEllipse(Brushes.White, drawX, drawY, StoneR * 2, StoneR * 2);
					g.DrawEllipse(Pens.Black, drawX, drawY, StoneR * 2, StoneR * 2);
					if(showNumbers < 0) {
						g.DrawString((p + 1).ToString(), font2, Brushes.Black, drawX + offsetX, drawY + offsetY);
					} else {
						if(FormRenju.MovePointer - p < showNumbers) {
							var showNumber = p + showNumbers - FormRenju.MovePointer;
							var pLength2 = showNumber.ToString().Length;
							var pLengthOffset2 = pLength2 * (fontSize - 2) / 2;
							var offsetX2 = -pLengthOffset2 + offsetX1[pLength2];
							g.DrawString(showNumber.ToString(), font2, Brushes.Black, drawX + offsetX2, drawY + offsetY);
						}
					}
					break;
				case Stone.None:
					// Other object
					break;
				}
			}
			// Draw other object
			string[] moveTypeString = { "５", "□", "４", "３", "禁" , "43" , "33", "44"};
			for(int y = 0; y < BoardSize; ++y) {
				for(int x = 0; x < BoardSize; ++x) {
					if(FormRenju.Point(x, y) != Stone.None) continue;
					var moveType = FormRenju.GetMoveType(Renju.ConvertMove2to1(x, y));
					if(moveType == -1) continue;
					var drawX = blockSize * x + StoneR + 1 + boardOffset;
					var drawY = blockSize * y + StoneR + 1 + boardOffset;
					var offsetX = -fontSize + fontSize * 19 / 16;
					var offsetY = fontSize / 2;
					g.DrawString(moveTypeString[moveType], font, (moveType <= 4 ? Brushes.Red : Brushes.Blue), drawX + offsetX, drawY + offsetY);
				}
			}
			g.Dispose();
			PictureBox.Image = canvas;
		// Redraw status-bar
			LastMoveStatusLabel.Text = FormRenju.GetLastMoveText();
			TurnPlayerStatusLabel.Text = FormRenju.GetTurnPlayerText();
			StepStatusLabel.Text = FormRenju.GetStepText();
		}

		private class Renju {
		// Member Variable
			public List<Stone> Board;
			public List<int> Move;
			public int MovePointer;
		// Method
			// Constructor
			public Renju() {
				Board = new List<Stone>(Enumerable.Repeat(Stone.None, BoardSize * BoardSize));
				Move = new List<int>();
				MovePointer = -1;
			}
			public Renju(string FileName) {
				Board = new List<Stone>(Enumerable.Repeat(Stone.None, BoardSize * BoardSize));
				Move = new List<int>();
				MovePointer = -1;
				System.IO.StreamReader sr = new System.IO.StreamReader(FileName, System.Text.Encoding.GetEncoding("utf-8"));
				var fileData = sr.ReadToEnd();
				sr.Close();
				switch(Path.GetExtension(FileName)) {
				case ".ban":
					{
						var getLine = fileData.Replace("\r\n", "\n").Split('\n');
						var setStone = Stone.Black;
						foreach(var str in getLine) {
							if(str.Length < 3)
								break;
							var x = int.Parse(str.Substring(0, str.Length - 2)) - 1;
							var y = int.Parse(str.Substring(str.Length - 2, 2)) - 1;
							if(x < 0 || y < 0)
								break;
							var move = ConvertMove2to1(x, y);
							Move.Add(move);
							Board[move] = setStone;
							setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
						}
						MovePointer = Move.Count - 1;
					}
					break;
				case ".dt5":
					Parse(fileData);
					break;
				case ".lws":
					{
						const string PositionString = "ABCDEFGHIJKLMNO";
						var moveStr = fileData.Split(' ');
						var setStone = Stone.Black;
						foreach(var str in moveStr) {
							var x = PositionString.IndexOf(str[0]);
							var y = BoardSize - int.Parse(str.Substring(1));
							if(x < 0)
								break;
							var move = ConvertMove2to1(x, y);
							Move.Add(move);
							Board[move] = setStone;
							setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
						}
						MovePointer = Move.Count - 1;
					}
					break;
				case ".rnj":
					{
						var getLine = fileData.Replace("\r\n", "\n").Split('\n');
						foreach(var str in getLine) {
							var info = str.Split(' ');
							if(info.Count() < 2)
								break;
							var x1 = int.Parse(info[1]);
							var y1 = int.Parse(info[0]);
							if(x1 < 0 || y1 < 0)
								break;
							var move1 = ConvertMove2to1(x1, y1);
							Move.Add(move1);
							Board[move1] = Stone.Black;
							if(info.Count() < 4)
								break;
							var x2 = int.Parse(info[3]);
							var y2 = int.Parse(info[2]);
							if(x2 < 0 || y2 < 0)
								break;
							var move2 = ConvertMove2to1(x2, y2);
							Move.Add(move2);
							Board[move2] = Stone.White;
						}
						MovePointer = Move.Count - 1;
					}
					break;
				case ".kif":
					{
						var getLine = fileData.Replace("\r\n", "\n").Split('\n');
						var step = 0;   //Now reading line
						Stone setStone = Stone.Black;
						foreach(var str in getLine) {
							if(step == 0) {
								/* Format : "* /t=X"
								 * X is number of turn player.
								 * But, this software ignore this line's information.
								 * 書式：「* /t=X」
								 * tの値は「次にどちらの手番か」ということだが、
								 * 本ソフトの実装では便宜上着手数によって
								 * 自動で手番が決まるため無視する
								 */
							}else if(step <= BoardSize) {
								/* Format : [\.12]{15}
								 * ".", "1", "2" = Blank, Black, White stone
								 * 盤面情報で、「.」「1」「2」が「空白」「黒石」「白石」に対応。
								 * 対局途中から打つためのものだが、指し手を徹頭徹尾再現するのは
								 * 不可能なので、スパイラルサーチによって擬似的に再現する
								 */
								// Read text
								for(int i = 0; i < BoardSize; ++i) {
									var stoneData = str.Substring(i, 1);
									if(stoneData == "1") {
										// Black stone
										Board[ConvertMove2to1(i, step - 1)] = Stone.Black;
									}else if(stoneData == "2") {
										// White stone
										Board[ConvertMove2to1(i, step - 1)] = Stone.White;
									}
								}
								if(step == BoardSize) {
									// Moves are reproduced by spiral search.
									// 読み終わったら、手順をスパイラルサーチによって再現する
									int xPos = 7, yPos = 7, moves = 1;
									var blackPos = new List<int>();
									var whitePos = new List<int>();
									if(Point(xPos, yPos) == Stone.Black) blackPos.Add(ConvertMove2to1(xPos, yPos));
									if(Point(xPos, yPos) == Stone.White) whitePos.Add(ConvertMove2to1(xPos, yPos));
									var exitFlg = false;
									while(true) {
										// Down
										for(int i = 0; i < moves; ++i) {
											++yPos;
											if(yPos >= BoardSize) {exitFlg = true; break;}
											if(Point(xPos, yPos) == Stone.Black) blackPos.Add(ConvertMove2to1(xPos, yPos));
											if(Point(xPos, yPos) == Stone.White) whitePos.Add(ConvertMove2to1(xPos, yPos));
										}
										if(exitFlg) break;
										// Left
										for(int i = 0; i < moves; ++i) {
											--xPos;
											if(xPos < 0) { exitFlg = true; break; }
											if(Point(xPos, yPos) == Stone.Black) blackPos.Add(ConvertMove2to1(xPos, yPos));
											if(Point(xPos, yPos) == Stone.White) whitePos.Add(ConvertMove2to1(xPos, yPos));
										}
										if(exitFlg) break;
										++moves;
										// Up
										for(int i = 0; i < moves; ++i) {
											--yPos;
											if(yPos < 0) {exitFlg = true; break;}
											if(Point(xPos, yPos) == Stone.Black) blackPos.Add(ConvertMove2to1(xPos, yPos));
											if(Point(xPos, yPos) == Stone.White) whitePos.Add(ConvertMove2to1(xPos, yPos));
										}
										if(exitFlg) break;
										// Right
										for(int i = 0; i < moves; ++i) {
											++xPos;
											if(xPos >= BoardSize) {exitFlg = true; break;}
											if(Point(xPos, yPos) == Stone.Black) blackPos.Add(ConvertMove2to1(xPos, yPos));
											if(Point(xPos, yPos) == Stone.White) whitePos.Add(ConvertMove2to1(xPos, yPos));
										}
										if(exitFlg) break;
										++moves;
									}
									while(true) {
										if(blackPos.Count == 0) break;
										Move.Add(blackPos[0]);
										blackPos.RemoveAt(0);
										if(whitePos.Count == 0) break;
										Move.Add(whitePos[0]);
										whitePos.RemoveAt(0);
									}
									setStone = (Move.Count % 2 == 0 ? Stone.Black : Stone.White);
								}
							} else if(str.IndexOf(",") != -1) {
								var splitStr = str.Split(',');
								var move = int.Parse(splitStr[0]);
								var moveX = (move % (BoardSize + 1)) - 1;
								var moveY = (move / (BoardSize + 1)) - 1;
								var moveXY = ConvertMove2to1(moveX, moveY);
								Board[moveXY] = setStone;
								Move.Add(moveXY);
								setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
							}
							++step;
						}
						MovePointer = Move.Count - 1;
					}
					break;
				case ".nrf": 
					{
						var getLine = fileData.Replace("\r\n", "\n").Split('\n');
						var step = 0;   //Now reading line
						Stone setStone = Stone.Black;
						const string PositionString = "123456789abcdef";
						foreach(var str in getLine) {
							if(step == 0) {
								/* title */
							} else if(str.Length >= 2) {
								var moveX = PositionString.IndexOf(str.Substring(0, 1));
								var moveY = PositionString.IndexOf(str.Substring(1, 1));
								if(moveX == -1 || moveY == -1) break;
								var moveXY = ConvertMove2to1(moveX, moveY);
								Board[moveXY] = setStone;
								Move.Add(moveXY);
								setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
							} else {
								break;
							}
							++step;
						}
						MovePointer = Move.Count - 1;
					}
					break;
				}
				return;
			}
			// Write file
			public void SaveFile(string FileName){
				// 出力用文字列
				var fileData = "";
				switch(Path.GetExtension(FileName)) {
				case ".ban":
					{
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							fileData += (moveXY[0] + 1).ToString() + (moveXY[1] + 1).ToString("D2") + "\r\n";
						}
					}
					break;
				case ".dt5":
					{
						fileData = ToStringMove();
					}
					break;
				case ".lws":
					{
						const string PositionString = "ABCDEFGHIJKLMNO";
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							if(p != 0)
								fileData += " ";
							fileData += PositionString[moveXY[0]] + (BoardSize - moveXY[1]).ToString();
						}
					}
					break;
				case ".rnj":
					{
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							fileData += moveXY[1].ToString() + " " + moveXY[0].ToString();
							fileData += (p % 2 == 0 ? " " : "\r\n");
						}
					}
					break;
				case ".kif":
					{
						fileData += "* /t=1\r\n";
						for(int i = 0; i < BoardSize; ++i) {
							for(int j = 0; j < BoardSize; ++j) {
								fileData += ".";
							}
							fileData += "\r\n";
						}
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							var move = (moveXY[0] + 1) + 16 * (moveXY[1] + 1);
							fileData += move.ToString() + ",人\r\n";
						}
					}
					break;
				case ".nrf":
					{
						const string PositionString = "123456789abcdef";
						fileData += Path.GetFileNameWithoutExtension(FileName) + "\r\n";
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							fileData += PositionString.Substring(moveXY[0], 1) + PositionString.Substring(moveXY[1], 1) + "\r\n";
						}
						fileData += "0\r\n";
					}
					break;
				}
				var sw = new StreamWriter(FileName, false, Encoding.GetEncoding("shift-jis"));
				sw.Write(fileData);
				sw.Close();
			}
			// Convert board into string
			// 盤面を文字列に変換する
			public string ToStringBoard() {
				// Star's position
				List<int> StarPosition = new List<int> { ConvertMove2to1(3, 3), ConvertMove2to1(11, 3), ConvertMove2to1(11, 11), ConvertMove2to1(3, 11), ConvertMove2to1(7, 7) };
				var output = "";
				for(int y = 0; y < BoardSize; ++y) {
					for(int x = 0; x < BoardSize; ++x) {
						var position = ConvertMove2to1(x, y);
						switch(Board[position]) {
						case Stone.None:
							if(x == 0 && y == 0) {
								output += "┌";
							} else if(x == BoardSize - 1 && y == 0) {
								output += "┐";
							} else if(x == BoardSize - 1 && y == BoardSize - 1) {
								output += "┘";
							} else if(x == 0 && y == BoardSize - 1) {
								output += "└";
							} else if(y == 0) {
								output += "┬";
							} else if(x == BoardSize - 1) {
								output += "┤";
							} else if(y == BoardSize - 1) {
								output += "┴";
							} else if(x == 0) {
								output += "├";
							} else if(StarPosition.IndexOf(position) != -1) {
								output += "╋";
							} else {
								output += "┼";
							}
							break;
						case Stone.Black:
							output += "●";
							break;
						case Stone.White:
							output += "○";
							break;
						}
					}
					output += "\n";
				}
				return output;
			}
			// Convert move into string
			private string ToStringMoveMini(int MovePosition) {
				var moveXY = ConvertMove1to2(MovePosition);
				return PositionStringX.Substring(moveXY[0], 1) + PositionStringY.Substring(moveXY[1], 1);
			}
			public string ToStringMove() {
				var output = "";
				for(int p = 0; p <= MovePointer; ++p) {
					output += ToStringMoveMini(Move[p]) + ",";
				}
				output += "**,";
				return output;
			}
			// Convert string into move
			public void Parse(string input) {
				Board = new List<Stone>(Enumerable.Repeat(Stone.None, BoardSize * BoardSize));
				Move = new List<int>();
				MovePointer = -1;
				var moveStr = input.Split(',');
				var setStone = Stone.Black;
				foreach(var str in moveStr) {
					var x = PositionStringX.IndexOf(str[0]);
					var y = PositionStringY.IndexOf(str[1]);
					if(x < 0 || y < 0)
						break;
					var move = ConvertMove2to1(x, y);
					Move.Add(move);
					Board[move] = setStone;
					setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
				}
				MovePointer = Move.Count - 1;
			}
			// Put Board String
			public void PutBoard() {
				MessageBox.Show(ToStringBoard(), SoftName, MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
			// Convert format of position
			static public int ConvertMove2to1(int MoveX, int MoveY) {
				return MoveY * BoardSize + MoveX;
			}
			static public int[] ConvertMove1to2(int MovePosition) {
				return new int[]{ MovePosition % BoardSize, MovePosition / BoardSize };
			}
			// Turn player information
			public Stone TurnPlayer() {
				return (MovePointer % 2 == 0 ? Stone.White : Stone.Black);
			}
			// Board information
			public Stone Point(int p) {
				return Board[Move[p]];
			}
			public Stone Point(int x, int y) {
				return Board[Renju.ConvertMove2to1(x, y)];
			}
			// Next move, and prev move
			public void Next() {
				if(MovePointer >= Move.Count - 1)
					return;
				var setStone = TurnPlayer();
				++MovePointer;
				Board[Move[MovePointer]] = setStone;
			}
			public void Prev() {
				if(MovePointer < 0)
					return;
				Board[Move[MovePointer]] = Stone.None;
				--MovePointer;
			}
			// Get line string
			public string GetLineString(int MovePosition) {
				var lineString = "#";
				var MoveXY = ConvertMove1to2(MovePosition);
				const string StoneString = "012" ;
				// Row
				for(int k = Math.Max(MoveXY[0] - 5, 0); k <= Math.Min(MoveXY[0] + 5, BoardSize - 1); ++k) {
					lineString += StoneString[(int)Board[ConvertMove2to1(k, MoveXY[1])]];
				}
				lineString += "#";
				// Column
				for(int k = Math.Max(MoveXY[1] - 5, 0); k <= Math.Min(MoveXY[1] + 5, BoardSize - 1); ++k) {
					lineString += StoneString[(int)Board[ConvertMove2to1(MoveXY[0], k)]];
				}
				lineString += "#";
				// Slant
				for(int k = -5; k <= 5; ++k) {
					var MoveX2 = MoveXY[0] + k;
					if(MoveX2 < 0 || MoveX2 >= BoardSize) continue;
					var MoveY2 = MoveXY[1] + k;
					if(MoveY2 < 0 || MoveY2 >= BoardSize) continue;
					lineString += StoneString[(int)Board[ConvertMove2to1(MoveX2, MoveY2)]];
				}
				lineString += "#";
				for(int k = -5; k <= 5; ++k) {
					var MoveX2 = MoveXY[0] - k;
					if(MoveX2 < 0 || MoveX2 >= BoardSize) continue;
					var MoveY2 = MoveXY[1] + k;
					if(MoveY2 < 0 || MoveY2 >= BoardSize) continue;
					lineString += StoneString[(int)Board[ConvertMove2to1(MoveX2, MoveY2)]];
				}
				lineString += "#";
				return lineString;
			}
			// Judge move type
			public int GetMoveType(int MovePosition) {
				var MoveXY = ConvertMove1to2(MovePosition);
				int count4_1 = 0, count4_2 = 0, count3 = 0; //Sum of Shi-ren, Katsu-shi and Katsu-san
				var rightOffsetX = new List<int> {  1,  1,  1,  0 };
				var rightOffsetY = new List<int> { -1,  0,  1,  1 };
				var leftOffsetX  = new List<int> { -1, -1, -1,  0 };
				var leftOffsetY  = new List<int> {  1,  0, -1, -1 };
				const string StoneString = "012";
				var myStone = TurnPlayer();
				var myStoneString = StoneString.Substring((int)myStone, 1);
				for(int k = 0; k < 4; ++k) {
					// Set right pattern
					var rightPattern = "";
					for(int x = MoveXY[0] + rightOffsetX[k], y = MoveXY[1] + rightOffsetY[k];
						0 <= x && x < BoardSize && 0 <= y && y < BoardSize;
						x += rightOffsetX[k], y += rightOffsetY[k]) {
						rightPattern += StoneString.Substring((int)Board[ConvertMove2to1(x, y)], 1);
					}
					// Set left pattern
					var leftPattern = "";
					for(int x = MoveXY[0] + leftOffsetX[k], y = MoveXY[1] + leftOffsetY[k];
						0 <= x && x < BoardSize && 0 <= y && y < BoardSize;
						x += leftOffsetX[k], y += leftOffsetY[k]) {
						leftPattern = StoneString.Substring((int)Board[ConvertMove2to1(x, y)], 1) + leftPattern;
					}
					// Quintuplex(Go-ren)
					if(myStone == Stone.Black) {
						var patternString = SubStr(leftPattern, -1, 5) + myStoneString + SubStr(rightPattern, 0, 5);
						if(Regex.IsMatch(patternString, "[^1]1{5}[^1]")) {
							return 0;
						}
					} else {
						var patternString = SubStr(leftPattern, -1, 4) + myStoneString + SubStr(rightPattern, 0, 4);
						if(Regex.IsMatch(patternString, "2{5}")) {
							return 0;
						}
					}
					// Too long multiplex(Cho-ren)
					// Cho-ren of black is Prohibited move(Kinsyu).
					if(myStone == Stone.Black) {
						var patternString = SubStr(leftPattern, -1, 5) + myStoneString + SubStr(rightPattern, 0, 5);
						if(Regex.IsMatch(patternString, "1{6}")) {
							return 4;
						}
					}
					/* If you make some Quadruplex of black, it's Prohibited move(Kinsyu) "Shi-Shi".
					 * 1. BO|BBB|OB   "Ryoutou-no-ShiShi"
					 * 2. BBO|BB|OBB  "Chouda-no-ShiShi"
					 * 3. BBBO|B|OBBB "Souryu-no-ShiShi"
					 * 一直線上に出来る四々で、両頭の四々・長蛇の四々・双龍の四々
					 */
					if(myStone == Stone.Black) {
						var patternString = SubStr(leftPattern, -1, 5) + "X" + SubStr(rightPattern, 0, 5);
						if(Regex.IsMatch(patternString, "[^1]10X1101[^1]")
						|| Regex.IsMatch(patternString, "[^1]101X101[^1]")
						|| Regex.IsMatch(patternString, "[^1]1011X01[^1]")
						|| Regex.IsMatch(patternString, "[^1]110X1011[^1]")
						|| Regex.IsMatch(patternString, "[^1]1101X011[^1]")
						|| Regex.IsMatch(patternString, "[^1]1110X0111[^1]")) {
							return 4;
						}
					}else {
						var patternString = SubStr(leftPattern, -1, 5) + "X" + SubStr(rightPattern, 0, 5);
						if(Regex.IsMatch(patternString, "20X2202")
						|| Regex.IsMatch(patternString, "202X202")
						|| Regex.IsMatch(patternString, "2022X02")
						|| Regex.IsMatch(patternString, "220X2022")
						|| Regex.IsMatch(patternString, "2202X022")
						|| Regex.IsMatch(patternString, "2220X0222")) {
							count4_2 += 2;
							continue;
						}
					}
					// Strong Quadruplex(Shi-ren)
					if(myStone == Stone.Black) {
						var patternString = SubStr(leftPattern, -1, 5) + myStoneString + SubStr(rightPattern, 0, 5);
						if(Regex.IsMatch(patternString, "[^1]01{4}0[^1]")) {
							++count4_1;
							continue;
						}
					} else {
						var patternString = SubStr(leftPattern, -1, 4) + myStoneString + SubStr(rightPattern, 0, 4);
						if(Regex.IsMatch(patternString, "02{4}0")) {
							++count4_1;
							continue;
						}
					}
					// Normal Quadruplex(Katsu-shi)
					var KatsushiPattern = new List <string>{
						myStoneString + "{4}0",
						myStoneString + "{3}0" + myStoneString,
						myStoneString + "{2}0" + myStoneString + "{2}",
						myStoneString + "0" + myStoneString + "{3}",
						"0" + myStoneString + "{4}"
					};
					if(myStone == Stone.Black) {
						var patternString = SubStr(leftPattern, -1, 5) + myStoneString + SubStr(rightPattern, 0, 5);
						bool count4_2_flg = false;
						for(int p = 0; p < KatsushiPattern.Count; ++p) {
							if(Regex.IsMatch(patternString, "[^1]" + KatsushiPattern[p] + "[^1]")) {
								count4_2_flg = true;
								break;
							}
						}
						if(count4_2_flg) {
							++count4_2;
							continue;
						}
					} else {
						var patternString = SubStr(leftPattern, -1, 4) + myStoneString + SubStr(rightPattern, 0, 4);
						bool count4_2_flg = false;
						for(int p = 0; p < KatsushiPattern.Count; ++p) {
							if(Regex.IsMatch(patternString, KatsushiPattern[p])) {
								count4_2_flg = true;
								break;
							}
						}
						if(count4_2_flg) {
							++count4_2;
							continue;
						}
					}
					// Strong Triplex(Katsu-san)
					var KatsusanPattern = new List<string>{
						"0" + myStoneString + "{3}00",
						"0" + myStoneString + "{2}0" + myStoneString + "0",
						"0" + myStoneString + "0" + myStoneString + "{2}0",
						"00" + myStoneString + "{3}0"
					};
					if(myStone == Stone.Black) {
						var leftPattern2 = SubStr(leftPattern, -1, 5);
						var rightPattern2 = SubStr(rightPattern, 0, 5);
						var patternString = leftPattern2 + myStoneString + rightPattern2;
						bool count3_flg = false;
						for(int p = 0; p < KatsusanPattern.Count; ++p) {
							if(Regex.IsMatch(patternString, "[^1]" + KatsusanPattern[p] + "[^1]")) {
								/* If you can't make Quadruplex to move, this pattern isn't Triplex.
								 * If you were to check this trap, you would mistake to judge Prohibited move "San-San".
								 * (This check is called "Ina-San-San".)
								 * 011100 0111X0 [^1]0$ + ^11X0[^1] , [^1]01$ + ^1X0[^1] , [^1]011$ + ^X0[^1]
								 * 011010 011X10 [^1]0$ + ^1X10[^1] , [^1]01$ + ^X10[^1] , [^1]011X$ + ^0[^1]
								 * 010110 01X110 [^1]0$ + ^X110[^1] , [^1]01X$ + ^10[^1] , [^1]01X1$ + ^0[^1]
								 * 001110 0X1110 [^1]0X$ + ^110[^1] , [^1]0X1$ + ^10[^1] , [^1]0X11$ + ^0[^1]
								 * 達四にできない三は三ではない。これは、打ちたい場所が禁点である場合も同様である。
								 * したがって、三を判定する際は、禁点絡みかどうかをチェックする必要がある。
								 * そのため、一旦今の場所に黒石を仮置きし、その上でその三が禁点を含まないかをチェックする。
								 */
								int offset1 = 0, offset2 = 0;
								while(true) {
									//011100
									if(Regex.IsMatch(leftPattern2, "[^1]0$") && Regex.IsMatch(rightPattern2, "^1100[^1]")) {offset1 = 3; offset2 = -1; break;}
									if(Regex.IsMatch(leftPattern2, "[^1]01$") && Regex.IsMatch(rightPattern2, "^100[^1]")) {offset1 = 2; offset2 = -2; break;}
									if(Regex.IsMatch(leftPattern2, "[^1]011$") && Regex.IsMatch(rightPattern2, "^00[^1]")) {offset1 = 1; offset2 = -3; break;}
									//011010
									if(Regex.IsMatch(leftPattern2, "[^1]0$") && Regex.IsMatch(rightPattern2, "^1010[^1]")) {offset1 = 2; break;}
									if(Regex.IsMatch(leftPattern2, "[^1]01$") && Regex.IsMatch(rightPattern2, "^010[^1]")) {offset1 = 1; break;}
									if(Regex.IsMatch(leftPattern2, "[^1]0110$") && Regex.IsMatch(rightPattern2, "^0[^1]")) {offset1 = -1; break;}
									//010110
									if(Regex.IsMatch(leftPattern2, "[^1]0$") && Regex.IsMatch(rightPattern2, "^0110[^1]")) {offset1 = 1; break;}
									if(Regex.IsMatch(leftPattern2, "[^1]010$") && Regex.IsMatch(rightPattern2, "^10[^1]")) {offset1 = -1; break;}
									if(Regex.IsMatch(leftPattern2, "[^1]0101$") && Regex.IsMatch(rightPattern2, "^0[^1]")) {offset1 = -2; break;}
									//001110
									if(Regex.IsMatch(leftPattern2, "[^1]00$") && Regex.IsMatch(rightPattern2, "^110[^1]")) {offset1 = -1; offset2 = 3; break;}
									if(Regex.IsMatch(leftPattern2, "[^1]001$") && Regex.IsMatch(rightPattern2, "^10[^1]")) {offset1 = -2; offset2 = 2; break; }
									if(Regex.IsMatch(leftPattern2, "[^1]0011$") && Regex.IsMatch(rightPattern2, "^0[^1]")) {offset1 = -3; offset2 = 1; break; }
									break;
								}
								// Judge of pattern "Ina-San-San"
								if(offset2 != 0) {
									int x2 = MoveXY[0] + offset2 * rightOffsetX[k], y2 = MoveXY[1] + offset2 * rightOffsetY[k];
									Board[MovePosition] = myStone;
									var typeX = GetMoveType(ConvertMove2to1(x2, y2));
									Board[MovePosition] = Stone.None;
									if(typeX != 4) {
										count3_flg = true;
										break;
									}
								}
								if(offset1 != 0) {
									int x2 = MoveXY[0] + offset1 * rightOffsetX[k], y2 = MoveXY[1] + offset1 * rightOffsetY[k];
									Board[MovePosition] = myStone;
									var typeX = GetMoveType(ConvertMove2to1(x2, y2));
									Board[MovePosition] = Stone.None;
									if(typeX != 4) {
										count3_flg = true;
										break;
									}
								}
							}
						}
						if(count3_flg) {
							++count3;
							continue;
						}
					} else {
						var patternString = SubStr(leftPattern, -1, 4) + myStoneString + SubStr(rightPattern, 0, 4);
						bool count3_flg = false;
						for(int p = 0; p < KatsusanPattern.Count; ++p) {
							if(Regex.IsMatch(patternString, KatsusanPattern[p])) {
								count3_flg = true;
								break;
							}
						}
						if(count3_flg) {
							++count3;
							continue;
						}
					}
				}
				// If you make some Quadruplex of black, it's Prohibited move(Kinsyu) "Shi-Shi".
				if(count4_1 + count4_2 >= 2) return (myStone == Stone.Black ? 4 : 7);
				// If you make some Triplex of black, it's Prohibited move(Kinsyu) "San-San".
				if(count3 >= 2) return (myStone == Stone.Black ? 4 : 6);

				// Other Judge
				if(count4_1 + count4_2 == 1 && count3 == 1) return 5;
				if(count4_1 > 0) return 1;
				if(count4_2 > 0) return 2;
				if(count3 > 0) return 3;
				return -1;
			}
			// Set Stone
			public void SetMove(int MovePosition) {
				Board[MovePosition] = TurnPlayer();
				++MovePointer;
				Move.RemoveRange(MovePointer, Move.Count - MovePointer);
				Move.Add(MovePosition);
			}
			// make string for status-bar
			public string GetLastMoveText() {
				return "LastMove : " + (Move.Count == 0 || MovePointer == -1 ? "" : ToStringMoveMini(Move[MovePointer]));
			}
			public string GetTurnPlayerText() {
				return "Turn : " + (TurnPlayer() == Stone.Black ? "Black" : "White");
			}
			public string GetStepText() {
				return "Step : " + (MovePointer + 1).ToString() + " / " + Move.Count.ToString();
			}
			// make string for I401.exe
			public string GetArgumentString() {
				var BoardStringArray = new List<string> { "-", "*", "O" };
				string boardText = "";
				for(int y = 0; y < BoardSize; ++y) {
					for(int x = 0; x < BoardSize; ++x) {
						boardText += BoardStringArray[(int)Point(x, y)];
					}
				}
				string turnText = BoardStringArray[(int)TurnPlayer()];
				return boardText + " " + turnText;
			}
			// SubStr
			public string SubStr(string str, int i, int n) {
				if(i >= 0) {
					if(str.Length >= i + n) {
						return str.Substring(i, n);
					} else {
						return str + new string('#', n - str.Length);
					}
				} else if(str.Length >= n) {
					return str.Substring(str.Length - n);
				} else {
					return new string('#', n - str.Length) + str;
				}
			}
		}
	}
}
