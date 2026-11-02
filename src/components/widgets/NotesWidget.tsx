import React, { useState } from "react";
import {
  StickyNote,
  Plus,
  Trash2,
  CheckCircle2,
  Circle,
  Pin,
} from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";
import type { NoteItem } from "../../types";

const COLOR_MAP: Record<NoteItem["color"], { border: string; bg: string; text: string }> = {
  amber: {
    border: "border-amber-500/30",
    bg: "bg-amber-500/10",
    text: "text-amber-300",
  },
  emerald: {
    border: "border-emerald-500/30",
    bg: "bg-emerald-500/10",
    text: "text-emerald-300",
  },
  sky: {
    border: "border-sky-500/30",
    bg: "bg-sky-500/10",
    text: "text-sky-300",
  },
  violet: {
    border: "border-purple-500/30",
    bg: "bg-purple-500/10",
    text: "text-purple-300",
  },
  rose: {
    border: "border-rose-500/30",
    bg: "bg-rose-500/10",
    text: "text-rose-300",
  },
};

export default function NotesWidget() {
  const { notes, addNote, deleteNote, toggleNoteCompleted, togglePinNote } =
    useWidgets();

  const [isAdding, setIsAdding] = useState(false);
  const [newTitle, setNewTitle] = useState("");
  const [newContent, setNewContent] = useState("");
  const [selectedColor, setSelectedColor] = useState<NoteItem["color"]>("amber");

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!newTitle.trim() && !newContent.trim()) return;
    addNote(newTitle || "Quick Note", newContent, selectedColor);
    setNewTitle("");
    setNewContent("");
    setIsAdding(false);
  };

  return (
    <div className="glass-card flex h-full flex-col justify-between overflow-hidden rounded-2xl p-5">
      {/* Header */}
      <div className="flex items-center justify-between border-b border-white/[0.06] pb-3">
        <div className="flex items-center gap-2">
          <div className="flex h-7 w-7 items-center justify-center rounded-lg bg-amber-500/10 text-amber-400 border border-amber-500/20">
            <StickyNote size={15} />
          </div>
          <span className="text-xs font-semibold text-white">Scratchpad & Notes</span>
        </div>
        <button
          type="button"
          onClick={() => setIsAdding((prev) => !prev)}
          className="pressable-btn flex items-center gap-1 rounded-lg border border-white/[0.08] bg-white/[0.04] px-2 py-1 text-[11px] font-medium text-zinc-300 hover:bg-white/[0.08]"
        >
          <Plus size={12} />
          <span>{isAdding ? "Cancel" : "New Note"}</span>
        </button>
      </div>

      {/* Note Creation Form (Collapsible) */}
      {isAdding && (
        <form onSubmit={handleSubmit} className="my-2 rounded-xl border border-white/[0.08] bg-white/[0.03] p-3">
          <input
            type="text"
            placeholder="Title..."
            value={newTitle}
            onChange={(e) => setNewTitle(e.target.value)}
            className="w-full bg-transparent text-xs font-semibold text-white placeholder-zinc-500 outline-none"
          />
          <textarea
            placeholder="Write something..."
            value={newContent}
            onChange={(e) => setNewContent(e.target.value)}
            rows={2}
            className="mt-1 w-full resize-none bg-transparent text-xs text-zinc-300 placeholder-zinc-500 outline-none"
          />
          <div className="mt-2 flex items-center justify-between">
            {/* Color chips */}
            <div className="flex items-center gap-1.5">
              {(["amber", "sky", "emerald", "violet", "rose"] as const).map((color) => (
                <button
                  key={color}
                  type="button"
                  onClick={() => setSelectedColor(color)}
                  className={`h-4 w-4 rounded-full transition ${
                    selectedColor === color ? "ring-2 ring-white ring-offset-1 ring-offset-black" : "opacity-60"
                  } ${
                    color === "amber"
                      ? "bg-amber-400"
                      : color === "sky"
                      ? "bg-sky-400"
                      : color === "emerald"
                      ? "bg-emerald-400"
                      : color === "violet"
                      ? "bg-purple-400"
                      : "bg-rose-400"
                  }`}
                />
              ))}
            </div>
            <button
              type="submit"
              className="pressable-btn rounded-lg bg-amber-500 px-3 py-1 text-[11px] font-semibold text-black hover:bg-amber-400"
            >
              Add
            </button>
          </div>
        </form>
      )}

      {/* Notes List */}
      <div className="my-2 flex-1 space-y-2 overflow-y-auto pr-1">
        {notes.length === 0 ? (
          <div className="py-6 text-center text-xs text-zinc-500">
            No notes yet. Click "+ New Note" to write one!
          </div>
        ) : (
          notes.map((note) => {
            const style = COLOR_MAP[note.color] || COLOR_MAP.amber;
            return (
              <div
                key={note.id}
                className={`group relative rounded-xl border p-2.5 transition ${style.border} ${style.bg} ${
                  note.completed ? "opacity-60" : ""
                }`}
              >
                <div className="flex items-start justify-between gap-2">
                  <button
                    type="button"
                    onClick={() => toggleNoteCompleted(note.id)}
                    className="mt-0.5 text-zinc-400 hover:text-white"
                  >
                    {note.completed ? (
                      <CheckCircle2 size={13} className="text-emerald-400" />
                    ) : (
                      <Circle size={13} />
                    )}
                  </button>

                  <div className="min-w-0 flex-1">
                    <h4
                      className={`text-xs font-semibold ${style.text} ${
                        note.completed ? "line-through text-zinc-500" : ""
                      }`}
                    >
                      {note.title}
                    </h4>
                    {note.content && (
                      <p
                        className={`mt-0.5 text-[11px] text-zinc-300 leading-relaxed ${
                          note.completed ? "line-through text-zinc-500" : ""
                        }`}
                      >
                        {note.content}
                      </p>
                    )}
                  </div>

                  {/* Actions */}
                  <div className="flex items-center gap-1 opacity-0 transition group-hover:opacity-100">
                    <button
                      type="button"
                      onClick={() => togglePinNote(note.id)}
                      title="Pin note"
                      className={`p-1 text-zinc-400 hover:text-white ${
                        note.isPinned ? "text-amber-400 opacity-100" : ""
                      }`}
                    >
                      <Pin size={11} />
                    </button>
                    <button
                      type="button"
                      onClick={() => deleteNote(note.id)}
                      title="Delete note"
                      className="p-1 text-zinc-400 hover:text-rose-400"
                    >
                      <Trash2 size={11} />
                    </button>
                  </div>
                </div>
              </div>
            );
          })
        )}
      </div>

      {/* Footer */}
      <div className="flex items-center justify-between border-t border-white/[0.06] pt-2 text-[10px] text-zinc-500">
        <span>{notes.length} Active Notes</span>
        <span>Auto-synced</span>
      </div>
    </div>
  );
}
