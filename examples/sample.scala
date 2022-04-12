/** Factory for [[mypackage.Person]] instances. */
object Person {
  /** Creates a person with a given name and age.
   *
   *  @param name their name
   *  @param age the age of the person to create
   */
  def apply(name: String, age: Int) = {}

  /** Creates a person with a given name and birthdate
   *
   *  @param name their name
   *  @param birthDate the person's birthdate
   *  @return a new Person instance with the age determined by the
   *          birthdate and current date.
   */
  def apply(name: String, birthDate: java.util.Date) = {}
}}

private type FooParam = {
  val baz: List[String => String]
  def bar(a: Int, b: Int): String
}


