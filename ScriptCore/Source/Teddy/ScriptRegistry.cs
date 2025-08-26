using System;
using System.Collections.Generic;
using System.Reflection;

namespace Teddy
{
    public static class ScriptRegistry
    {
        private static Dictionary<string, ScriptClass> s_ScriptClasses = new Dictionary<string, ScriptClass>();

        public static void RegisterScripts()
        {
            s_ScriptClasses.Clear();

            Assembly assembly = Assembly.GetExecutingAssembly();
            foreach (Type type in assembly.GetTypes())
            {
                if (type.IsSubclassOf(typeof(Entity)))
                {
                    s_ScriptClasses[type.FullName] = new ScriptClass(type);
                }
            }
        }

        public static ScriptClass GetScriptClass(string name)
        {
            if (s_ScriptClasses.TryGetValue(name, out ScriptClass scriptClass))
                return scriptClass;
            return null;
        }
    }

    public class ScriptClass
    {
        public Type Type { get; private set; }
        public Dictionary<string, FieldInfo> Fields { get; private set; }

        public ScriptClass(Type type)
        {
            Type = type;
            Fields = new Dictionary<string, FieldInfo>();

            foreach (var field in type.GetFields(BindingFlags.Public | BindingFlags.Instance))
            {
                if (field.GetCustomAttribute<ShowInEditorAttribute>() != null)
                {
                    Fields[field.Name] = field;
                }
            }
        }
    }
}
