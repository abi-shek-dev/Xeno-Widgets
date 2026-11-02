import React, { useState } from "react";
import { StickyNote, Plus, Trash2, CheckCircle2, Circle, Pin } from "lucide-react";
import { useWidgets } from "../../context/WidgetContext";

const widgetContainer: React.CSSProperties = {
  height: "100%",
  display: "flex",
  flexDirection: "column",
  border: "1px solid var(--xe-border)",
  borderRadius: 4,
  background: "var(--xe-surface)",
  overflow: "hidden",
};

export default function NotesWidget() {
  const { notes, addNote, deleteNote, toggleNoteCompleted, togglePinNote } = useWidgets();
  const [isAdding, setIsAdding] = useState(false);
  const [newTitle, setNewTitle] = useState("");
  const [newContent, setNewContent] = useState("");

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!newTitle.trim() && !newContent.trim()) return;
    addNote(newTitle || "Quick Note", newContent, "amber");
    setNewTitle("");
    setNewContent("");
    setIsAdding(false);
  };

  return (
    <div style={widgetContainer}>
      {/* Header */}
      <div
        style={{
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          padding: "8px 12px",
          borderBottom: "1px solid var(--xe-border)",
          flexShrink: 0,
        }}
      >
        <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
          <StickyNote size={13} style={{ color: "var(--xe-muted)" }} />
          <span
            style={{
              fontFamily: "var(--xe-font-mono)",
              fontSize: 9,
              color: "var(--xe-faint)",
              letterSpacing: "0.08em",
              textTransform: "uppercase",
            }}
          >
            SCRATCHPAD
          </span>
        </div>
        <button
          type="button"
          onClick={() => setIsAdding((prev) => !prev)}
          style={{
            display: "flex",
            alignItems: "center",
            gap: 3,
            padding: "3px 8px",
            fontSize: 10,
            fontFamily: "var(--xe-font-mono)",
            letterSpacing: "0.04em",
            cursor: "pointer",
            background: isAdding ? "var(--xe-fg)" : "transparent",
            color: isAdding ? "var(--xe-bg)" : "var(--xe-muted)",
            border: "1px solid var(--xe-border)",
            borderRadius: 3,
          }}
        >
          <Plus size={10} />
          {isAdding ? "CANCEL" : "NEW"}
        </button>
      </div>

      {/* Add form */}
      {isAdding && (
        <form
          onSubmit={handleSubmit}
          style={{
            padding: "10px 12px",
            borderBottom: "1px solid var(--xe-border)",
            background: "var(--xe-bg)",
            flexShrink: 0,
          }}
        >
          <input
            type="text"
            placeholder="Title..."
            value={newTitle}
            onChange={(e) => setNewTitle(e.target.value)}
            style={{
              width: "100%",
              background: "transparent",
              border: "none",
              outline: "none",
              fontSize: 12,
              fontFamily: "var(--xe-font-body)",
              fontWeight: 500,
              color: "var(--xe-fg)",
            }}
          />
          <textarea
            placeholder="Write something..."
            value={newContent}
            onChange={(e) => setNewContent(e.target.value)}
            rows={2}
            style={{
              width: "100%",
              marginTop: 4,
              background: "transparent",
              border: "none",
              outline: "none",
              resize: "none",
              fontSize: 11,
              fontFamily: "var(--xe-font-body)",
              color: "var(--xe-muted)",
            }}
          />
          <div
            style={{
              display: "flex",
              alignItems: "center",
              justifyContent: "flex-end",
              marginTop: 6,
            }}
          >
            <button
              type="submit"
              style={{
                padding: "3px 12px",
                fontSize: 10,
                fontFamily: "var(--xe-font-mono)",
                letterSpacing: "0.04em",
                cursor: "pointer",
                background: "var(--xe-fg)",
                color: "var(--xe-bg)",
                border: "none",
                borderRadius: 3,
              }}
            >
              ADD
            </button>
          </div>
        </form>
      )}

      {/* Notes list */}
      <div style={{ flex: 1, overflowY: "auto", padding: "8px 12px" }}>
        {notes.length === 0 ? (
          <div
            style={{
              padding: "24px 0",
              textAlign: "center",
              fontSize: 11,
              color: "var(--xe-faint)",
              fontFamily: "var(--xe-font-body)",
            }}
          >
            No notes yet.
          </div>
        ) : (
          <div style={{ display: "flex", flexDirection: "column", gap: 4 }}>
            {notes.map((note) => (
              <div
                key={note.id}
                className="group"
                style={{
                  padding: "8px 10px",
                  border: "1px solid var(--xe-border)",
                  borderRadius: 3,
                  background: "var(--xe-bg)",
                  opacity: note.completed ? 0.55 : 1,
                }}
              >
                <div style={{ display: "flex", alignItems: "flex-start", gap: 6 }}>
                  <button
                    type="button"
                    onClick={() => toggleNoteCompleted(note.id)}
                    style={{
                      marginTop: 1,
                      color: "var(--xe-faint)",
                      cursor: "pointer",
                      flexShrink: 0,
                    }}
                  >
                    {note.completed ? (
                      <CheckCircle2 size={12} style={{ color: "var(--xe-muted)" }} />
                    ) : (
                      <Circle size={12} />
                    )}
                  </button>
                  <div style={{ flex: 1, minWidth: 0 }}>
                    <h4
                      style={{
                        fontSize: 12,
                        fontFamily: "var(--xe-font-body)",
                        fontWeight: 500,
                        color: "var(--xe-fg)",
                        textDecoration: note.completed ? "line-through" : "none",
                        overflow: "hidden",
                        textOverflow: "ellipsis",
                        whiteSpace: "nowrap",
                      }}
                    >
                      {note.title}
                    </h4>
                    {note.content && (
                      <p
                        style={{
                          marginTop: 2,
                          fontSize: 10,
                          color: "var(--xe-muted)",
                          fontFamily: "var(--xe-font-body)",
                          lineHeight: 1.5,
                          textDecoration: note.completed ? "line-through" : "none",
                        }}
                      >
                        {note.content}
                      </p>
                    )}
                  </div>
                  <div style={{ display: "flex", gap: 2, flexShrink: 0 }}>
                    <button
                      type="button"
                      onClick={() => togglePinNote(note.id)}
                      style={{
                        color: note.isPinned ? "var(--xe-fg)" : "var(--xe-faint)",
                        cursor: "pointer",
                        padding: 2,
                      }}
                    >
                      <Pin size={10} />
                    </button>
                    <button
                      type="button"
                      onClick={() => deleteNote(note.id)}
                      style={{ color: "var(--xe-faint)", cursor: "pointer", padding: 2 }}
                    >
                      <Trash2 size={10} />
                    </button>
                  </div>
                </div>
              </div>
            ))}
          </div>
        )}
      </div>

      {/* Footer */}
      <div
        style={{
          display: "flex",
          justifyContent: "space-between",
          padding: "5px 12px",
          borderTop: "1px solid var(--xe-border)",
          flexShrink: 0,
        }}
      >
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
          }}
        >
          {notes.length} NOTES
        </span>
        <span
          style={{
            fontFamily: "var(--xe-font-mono)",
            fontSize: 9,
            color: "var(--xe-faint)",
          }}
        >
          AUTO-SAVED
        </span>
      </div>
    </div>
  );
}
