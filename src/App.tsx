
import Sidebar from "./components/layout/Sidebar";
import Dashboard from "./dashboard/Dashboard";

function App() {
  return (
    <div className="flex min-h-screen bg-[#eeeae2] text-[#171717]">
      <Sidebar />

      <div className="min-w-0 flex-1 bg-[#eeeae2]">
        <Dashboard />
      </div>
    </div>
  );
}

export default App;
