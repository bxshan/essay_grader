const express = require("express");
const mongoose = require("mongoose");
const path = require("path");
const { spawn } = require("child_process");

const app = express();

// Parse incoming request bodies as JSON or url encoded form data
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Serve the main HTML page and its client-side script
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "index.html"));
});
app.get("/script.js", (req, res) => {
  res.sendFile(path.join(__dirname, "script.js"));
});

// local host connect to mongoose: 
// if cannot connect, still proceed with grading, just without plagiarism detection
mongoose
  .connect("mongodb://127.0.0.1:27017/essaygrader")
  .then(() => console.log("mongoose db connected"))
  .catch((err) => {
    console.error("conenction error ", err.message);
    console.log("no plagiarism detection");
  });

// struct for storing essays
const essaySchema = new mongoose.Schema({
  name: String,
  essay: String,
  grade: Number,
  reasons: [String],
  submittedAt: { type: Date, default: Date.now },
});
const Essay = mongoose.model("Essay", essaySchema);

// spawn compiled c++ binary for grading
//
// ! STDIN FORMAT:
//   <current essay>\0<name1>\0<essay1>\0<name2>\0<essay2>\0...
//   seperated with null char 
//   dont need current student name for plagiarism
//
// ! STDOUT: single line of JSON: {"grade": ..., "reasons": [...]}
//   if plagiarism detected, exits grading early and just scores 0
function runGrader(essay, previousEssays) {
  return new Promise((resolve, reject) => {
    // binary name must be "grader"
    const child = spawn(path.join(__dirname, "grader"));
    let stdout = "";
    let stderr = "";
    child.stdout.on("data", (data) => { stdout += data; });
    child.stderr.on("data", (data) => { stderr += data; });
    child.on("close", (code) => {
      if (code !== 0) return reject(new Error(`g.cc exited with code ${code}: ${stderr}`));
      try {
        resolve(JSON.parse(stdout));
      } catch (e) {
        reject(new Error("grader stdout json cannot parse: " + stdout));
      }
    });
    child.on("error", reject);

    // write the current essay first, then each previous essay as a
    // name/essay pair seperated with \0
    child.stdin.write(essay);
    for (const prev of previousEssays) {
      child.stdin.write('\0');
      child.stdin.write(prev.name);
      child.stdin.write('\0');
      child.stdin.write(prev.essay);
    }
    child.stdin.end();
  });
}

// POST endpoint: receives {name, essay} from the client, grades it,
// stores result in MongoDB, then returns the grade + reasons as JSON.
app.post("/", async (req, res) => {
  try {
    const { name, essay } = req.body;

    if (!name || !essay) {
      return res.status(400).json({ error: "Name and essay are required." });
    }

    // fetch all previous essays to determine plagiarism
    // only need name + essay content
    let previousEssays = [];
    if (mongoose.connection.readyState === 1) { // skip if no db
      previousEssays = await Essay.find({}, "name essay").lean();
    }

    // run the c++ grader
    const result = await runGrader(essay, previousEssays);

    // keep this submission if db is not cooked so future submissions can check plagiarism
    if (mongoose.connection.readyState === 1) {
      await Essay.create({
        name,
        essay,
        grade: result.grade,
        reasons: result.reasons,
      });
    }

    // return name +, grade + reasons
    res.json({ name, grade: result.grade, reasons: result.reasons });
  } catch (err) {
    console.error("grading error:", err);
    res.status(500).json({ error: "500 error" });
  }
});

app.listen(2020, () => { console.log("Essay Grader begin at http://localhost:2020/"); });
