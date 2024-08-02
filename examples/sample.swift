
import RealmSwift

// Example Single Line Comment
struct ScrumsView: View {
   @ObservedResults(DailyScrum.self) var scrums
   @State private var isPresented = false
   @State private var newScrumData = DailyScrum.Data()
   @State private var currentScrum = DailyScrum()

   /**
	* Example multi-line comment
	*/
   var body: some View {
      List {
            if let scrums = scrums {
               ForEach(scrums) { scrum in
                  NavigationLink(destination: DetailView(scrum: scrum)) {
                        CardView(scrum: scrum)
                  }
                  .listRowBackground(scrum.color)
               }
            }
      }
      .navigationTitle("Daily Scrums")
      .navigationBarItems(trailing: Button(action: {
            isPresented = true
      }) {
            Image(systemName: "plus")
      })
      .sheet(isPresented: $isPresented) {
            NavigationView {
               EditView(scrumData: $newScrumData)
                  .navigationBarItems(leading: Button("Dismiss") {
                        isPresented = false
                  }, trailing: Button("Add") {
                        let newScrum = DailyScrum(
                           title: newScrumData.title,
                           attendees: newScrumData.attendees,
                           lengthInMinutes: Int(newScrumData.lengthInMinutes),
                           color: newScrumData.color)
                        $scrums.append(newScrum)
                        isPresented = false
                  })
            }
      }
   }
}
